#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generate a quantum-resistant key pair
 *
 * @param public_key Pointer to store the public key
 * @param public_key_len Pointer to store the length of the public key
 * @param secret_key Pointer to store the secret key
 * @param secret_key_len Pointer to store the length of the secret key
 * @return 0 on success, -1 on failure
 */
int generate_keypair(uint8_t **public_key, size_t *public_key_len,
                     uint8_t **secret_key, size_t *secret_key_len);

/**
 * Encrypt data using CRYSTALS-Kyber and AES-256-GCM
 *
 * @param public_key The public key
 * @param public_key_len Length of the public key
 * @param plaintext The data to encrypt
 * @param plaintext_len Length of the plaintext
 * @param ciphertext Pointer to store the encrypted data
 * @param ciphertext_len Pointer to store the length of the ciphertext
 * @return 0 on success, -1 on failure
 */
int encrypt(const uint8_t *public_key, size_t public_key_len,
            const uint8_t *plaintext, size_t plaintext_len,
            uint8_t **ciphertext, size_t *ciphertext_len);

/**
 * Decrypt data using CRYSTALS-Kyber and AES-256-GCM
 *
 * @param secret_key The secret key
 * @param secret_key_len Length of the secret key
 * @param ciphertext The data to decrypt
 * @param ciphertext_len Length of the ciphertext
 * @param plaintext Pointer to store the decrypted data
 * @param plaintext_len Pointer to store the length of the plaintext
 * @return 0 on success, -1 on failure
 */
int decrypt(const uint8_t *secret_key, size_t secret_key_len,
            const uint8_t *ciphertext, size_t ciphertext_len,
            uint8_t **plaintext, size_t *plaintext_len);

/**
 * Clean up and free memory
 *
 * @param ptr Pointer to the pointer of the memory to free
 */
void cleanup(void **ptr);

/**
 * Initialize the encryption module
 * This function should be called once at the start of the program
 */
void init_encryption(void);

/**
 * Cleanup the encryption module
 * This function should be called once at the end of the program
 */
void cleanup_encryption(void);

/**
 * Get the last error message from the encryption module
 *
 * @return The last error message
 */
const char* get_error(void);

#ifdef __cplusplus
}
#endif

#endif /* ENCRYPTION_H */
