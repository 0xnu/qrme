#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oqs/oqs.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "../include/encryption.h"
#include "../include/utils.h"

#define MAX_ERROR_LENGTH 256
#define AES_256_KEY_SIZE 32
#define GCM_IV_SIZE 12
#define GCM_TAG_SIZE 16

// Global error state
static char error_message[MAX_ERROR_LENGTH] = {0};

// Function to set error message
static void set_error(const char* message) {
    strncpy(error_message, message, MAX_ERROR_LENGTH - 1);
    error_message[MAX_ERROR_LENGTH - 1] = '\0';
}

// Function to get error message
const char* get_error() {
    return error_message;
}

int generate_keypair(uint8_t **public_key, size_t *public_key_len,
                     uint8_t **secret_key, size_t *secret_key_len) {
    OQS_KEM *kem = NULL;
    int ret = -1;

    kem = OQS_KEM_new(OQS_KEM_alg_kyber_768);
    if (kem == NULL) {
        set_error("Error creating KEM instance");
        return ret;
    }

    *public_key = secure_realloc(NULL, kem->length_public_key);
    *secret_key = secure_realloc(NULL, kem->length_secret_key);

    if (!*public_key || !*secret_key) {
        set_error("Error allocating memory for keys");
        goto cleanup;
    }

    if (OQS_KEM_keypair(kem, *public_key, *secret_key) != OQS_SUCCESS) {
        set_error("Error generating keypair");
        goto cleanup;
    }

    *public_key_len = kem->length_public_key;
    *secret_key_len = kem->length_secret_key;

    ret = 0;  // Success

cleanup:
    if (ret != 0) {
        secure_free((void**)public_key);
        secure_free((void**)secret_key);
    }
    OQS_KEM_free(kem);
    return ret;
}

int encrypt(const uint8_t *public_key, size_t public_key_len,
            const uint8_t *plaintext, size_t plaintext_len,
            uint8_t **ciphertext, size_t *ciphertext_len) {
    OQS_KEM *kem = NULL;
    EVP_CIPHER_CTX *ctx = NULL;
    uint8_t *kem_ciphertext = NULL;
    uint8_t *shared_secret = NULL;
    uint8_t *aes_ciphertext = NULL;
    uint8_t iv[GCM_IV_SIZE];
    uint8_t tag[GCM_TAG_SIZE];
    int len, aes_ciphertext_len;
    int ret = -1;

    kem = OQS_KEM_new(OQS_KEM_alg_kyber_768);
    if (kem == NULL) {
        set_error("Error creating KEM instance");
        return ret;
    }

    if (public_key_len != kem->length_public_key) {
        set_error("Invalid public key length");
        goto cleanup;
    }

    kem_ciphertext = secure_realloc(NULL, kem->length_ciphertext);
    shared_secret = secure_realloc(NULL, kem->length_shared_secret);
    aes_ciphertext = secure_realloc(NULL, plaintext_len + EVP_MAX_BLOCK_LENGTH);

    if (!kem_ciphertext || !shared_secret || !aes_ciphertext) {
        set_error("Error allocating memory");
        goto cleanup;
    }

    if (OQS_KEM_encaps(kem, kem_ciphertext, shared_secret, public_key) != OQS_SUCCESS) {
        set_error("Error in KEM encapsulation");
        goto cleanup;
    }

    // Generate a random IV
    if (RAND_bytes(iv, GCM_IV_SIZE) != 1) {
        set_error("Error generating random IV");
        goto cleanup;
    }

    // Create and initialise the context
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        set_error("Error creating cipher context");
        goto cleanup;
    }

    // Initialise the encryption operation
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, shared_secret, iv) != 1) {
        set_error("Error initializing encryption");
        goto cleanup;
    }

    // Encrypt plaintext
    if (EVP_EncryptUpdate(ctx, aes_ciphertext, &len, plaintext, plaintext_len) != 1) {
        set_error("Error in encryption update");
        goto cleanup;
    }
    aes_ciphertext_len = len;

    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, aes_ciphertext + len, &len) != 1) {
        set_error("Error finalizing encryption");
        goto cleanup;
    }
    aes_ciphertext_len += len;

    // Get the tag
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, GCM_TAG_SIZE, tag) != 1) {
        set_error("Error getting tag");
        goto cleanup;
    }

    // Assemble final ciphertext: KEM ciphertext + IV + AES ciphertext + tag
    *ciphertext_len = kem->length_ciphertext + GCM_IV_SIZE + aes_ciphertext_len + GCM_TAG_SIZE;
    *ciphertext = secure_realloc(NULL, *ciphertext_len);
    if (!*ciphertext) {
        set_error("Error allocating memory for final ciphertext");
        goto cleanup;
    }

    memcpy(*ciphertext, kem_ciphertext, kem->length_ciphertext);
    memcpy(*ciphertext + kem->length_ciphertext, iv, GCM_IV_SIZE);
    memcpy(*ciphertext + kem->length_ciphertext + GCM_IV_SIZE, aes_ciphertext, aes_ciphertext_len);
    memcpy(*ciphertext + kem->length_ciphertext + GCM_IV_SIZE + aes_ciphertext_len, tag, GCM_TAG_SIZE);

    ret = 0;  // Success

cleanup:
    if (ctx) EVP_CIPHER_CTX_free(ctx);
    secure_free((void**)&kem_ciphertext);
    secure_free((void**)&shared_secret);
    secure_free((void**)&aes_ciphertext);
    OQS_KEM_free(kem);
    return ret;
}

int decrypt(const uint8_t *secret_key, size_t secret_key_len,
            const uint8_t *ciphertext, size_t ciphertext_len,
            uint8_t **plaintext, size_t *plaintext_len) {
    OQS_KEM *kem = NULL;
    EVP_CIPHER_CTX *ctx = NULL;
    uint8_t *shared_secret = NULL;
    uint8_t iv[GCM_IV_SIZE];
    uint8_t tag[GCM_TAG_SIZE];
    int len;
    int ret = -1;

    printf("Debug: Initializing KEM\n");
    kem = OQS_KEM_new(OQS_KEM_alg_kyber_768);
    if (kem == NULL) {
        set_error("Error creating KEM instance");
        return ret;
    }

    printf("Debug: Checking key and ciphertext lengths\n");
    printf("Debug: secret_key_len: %zu, kem->length_secret_key: %zu\n", secret_key_len, kem->length_secret_key);
    printf("Debug: ciphertext_len: %zu, kem->length_ciphertext: %zu, GCM_IV_SIZE: %d, GCM_TAG_SIZE: %d\n",
           ciphertext_len, kem->length_ciphertext, GCM_IV_SIZE, GCM_TAG_SIZE);

    if (secret_key_len != kem->length_secret_key ||
        ciphertext_len <= kem->length_ciphertext + GCM_IV_SIZE + GCM_TAG_SIZE) {
        set_error("Invalid key or ciphertext length");
        goto cleanup;
    }

    printf("Debug: Allocating shared secret\n");
    shared_secret = secure_realloc(NULL, kem->length_shared_secret);
    if (!shared_secret) {
        set_error("Error allocating memory");
        goto cleanup;
    }

    printf("Debug: Performing KEM decapsulation\n");
    if (OQS_KEM_decaps(kem, shared_secret, ciphertext, secret_key) != OQS_SUCCESS) {
        set_error("Error in KEM decapsulation");
        goto cleanup;
    }

    printf("Debug: Extracting IV and tag\n");
    memcpy(iv, ciphertext + kem->length_ciphertext, GCM_IV_SIZE);
    memcpy(tag, ciphertext + ciphertext_len - GCM_TAG_SIZE, GCM_TAG_SIZE);

    printf("Debug: Creating cipher context\n");
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        set_error("Error creating cipher context");
        goto cleanup;
    }

    printf("Debug: Initializing decryption\n");
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, shared_secret, iv) != 1) {
        set_error("Error initializing decryption");
        goto cleanup;
    }

    printf("Debug: Setting expected tag\n");
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, GCM_TAG_SIZE, (void*)tag) != 1) {
        set_error("Error setting tag");
        goto cleanup;
    }

    printf("Debug: Allocating memory for plaintext\n");
    size_t aes_ciphertext_len = ciphertext_len - kem->length_ciphertext - GCM_IV_SIZE - GCM_TAG_SIZE;
    *plaintext = secure_realloc(NULL, aes_ciphertext_len);
    if (!*plaintext) {
        set_error("Error allocating memory for plaintext");
        goto cleanup;
    }

    printf("Debug: Decrypting ciphertext\n");
    if (EVP_DecryptUpdate(ctx, *plaintext, &len,
                          ciphertext + kem->length_ciphertext + GCM_IV_SIZE,
                          aes_ciphertext_len) != 1) {
        set_error("Error in decryption update");
        goto cleanup;
    }
    *plaintext_len = len;

    printf("Debug: Finalizing decryption\n");
    if (EVP_DecryptFinal_ex(ctx, *plaintext + len, &len) != 1) {
        set_error("Error finalizing decryption");
        goto cleanup;
    }
    *plaintext_len += len;

    ret = 0;  // Success

cleanup:
    if (ctx) EVP_CIPHER_CTX_free(ctx);
    secure_free((void**)&shared_secret);
    OQS_KEM_free(kem);
    if (ret != 0 && *plaintext) {
        secure_free((void**)plaintext);
    }
    return ret;
}

void cleanup(void **ptr) {
    if (ptr && *ptr) {
        secure_free(ptr);
    }
}

void init_encryption() {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
}

void cleanup_encryption() {
    EVP_cleanup();
    ERR_free_strings();
}
