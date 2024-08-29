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

typedef struct {
    size_t size;
    char data[];
} secure_alloc_t;

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

    *public_key = secure_malloc(kem->length_public_key);
    *secret_key = secure_malloc(kem->length_secret_key);

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

    kem_ciphertext = secure_malloc(kem->length_ciphertext);
    shared_secret = secure_malloc(kem->length_shared_secret);
    aes_ciphertext = secure_malloc(plaintext_len + EVP_MAX_BLOCK_LENGTH);

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
    *ciphertext = secure_malloc(*ciphertext_len);
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
    uint8_t *aes_ciphertext = NULL;
    uint8_t iv[GCM_IV_SIZE];
    uint8_t tag[GCM_TAG_SIZE];
    int len;
    int ret = -1;

    kem = OQS_KEM_new(OQS_KEM_alg_kyber_768);
    if (kem == NULL) {
        set_error("Error creating KEM instance");
        return ret;
    }

    if (secret_key_len != kem->length_secret_key ||
        ciphertext_len <= kem->length_ciphertext + GCM_IV_SIZE + GCM_TAG_SIZE) {
        set_error("Invalid key or ciphertext length");
        goto cleanup;
    }

    shared_secret = secure_malloc(kem->length_shared_secret);
    if (!shared_secret) {
        set_error("Error allocating memory");
        goto cleanup;
    }

    // Decapsulate to get the shared secret
    if (OQS_KEM_decaps(kem, shared_secret, ciphertext, secret_key) != OQS_SUCCESS) {
        set_error("Error in KEM decapsulation");
        goto cleanup;
    }

    // Extract IV and tag from ciphertext
    memcpy(iv, ciphertext + kem->length_ciphertext, GCM_IV_SIZE);
    memcpy(tag, ciphertext + ciphertext_len - GCM_TAG_SIZE, GCM_TAG_SIZE);

    // Create and initialise the context
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        set_error("Error creating cipher context");
        goto cleanup;
    }

    // Initialise the decryption operation
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, shared_secret, iv) != 1) {
        set_error("Error initializing decryption");
        goto cleanup;
    }

    // Set expected tag value
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, GCM_TAG_SIZE, (void*)tag) != 1) {
        set_error("Error setting tag");
        goto cleanup;
    }

    // Allocate memory for plaintext
    size_t aes_ciphertext_len = ciphertext_len - kem->length_ciphertext - GCM_IV_SIZE - GCM_TAG_SIZE;
    *plaintext = secure_malloc(aes_ciphertext_len);
    if (!*plaintext) {
        set_error("Error allocating memory for plaintext");
        goto cleanup;
    }

    // Decrypt ciphertext
    if (EVP_DecryptUpdate(ctx, *plaintext, &len,
                          ciphertext + kem->length_ciphertext + GCM_IV_SIZE,
                          aes_ciphertext_len) != 1) {
        set_error("Error in decryption update");
        goto cleanup;
    }
    *plaintext_len = len;

    // Finalize decryption
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

void cleanup(void *ptr) {
    if (ptr) {
        // Check if the pointer was allocated by our secure_malloc
        secure_alloc_t* alloc = (secure_alloc_t*)((char*)ptr - offsetof(secure_alloc_t, data));
        if (alloc->size > 0) {
            // This was allocated by our secure_malloc, so use secure_free
            secure_free(&ptr);
        } else {
            // This was likely allocated by the OQS library, so use regular free
            free(ptr);
        }
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
