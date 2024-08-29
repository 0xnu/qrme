# ISSUES

## 0.0.1 - 2024-08-29 - @0xnu

```sh
./qrme test_model.bin test_secret.key
secure_malloc: Allocated 344 bytes at 0x7fe267704580 (returned 0x7fe267704588)
Debug: Secret key length: 2400
Debug: Reading number of layers
Debug: Number of layers: 2
Debug: Reading layer 0 dimensions
Debug: Layer 0 dimensions: 2 x 3
Debug: Reading encrypted weights length for layer 0
Debug: Encrypted weights length for layer 0: 1140
secure_malloc: Allocated 1140 bytes at 0x7fe26780de00 (returned 0x7fe26780de08)
Debug: Reading encrypted weights for layer 0
Debug: Decrypting weights for layer 0 (encrypted_weights_len: 1140)
Debug: Initializing KEM
Debug: Checking key and ciphertext lengths
Debug: secret_key_len: 2400, kem->length_secret_key: 2400
Debug: ciphertext_len: 1140, kem->length_ciphertext: 1088, GCM_IV_SIZE: 12, GCM_TAG_SIZE: 16
Debug: Allocating shared secret
secure_malloc: Allocated 32 bytes at 0x6000007d8270 (returned 0x6000007d8278)
Debug: Performing KEM decapsulation
Debug: Extracting IV and tag
Debug: Creating cipher context
Debug: Initializing decryption
Debug: Setting expected tag
Debug: Allocating memory for plaintext
secure_malloc: Allocated 24 bytes at 0x6000009c2d40 (returned 0x6000009c2d48)
Debug: Decrypting ciphertext
Debug: Finalizing decryption
secure_free: Warning - cannot access memory before 0x6000007d8278
secure_free: Warning - cannot access memory before 0x7fe26780de08
Debug: Decrypted weights length for layer 0: 24
Debug: Reading layer 1 dimensions
Debug: Layer 1 dimensions: 1 x 2
Debug: Reading encrypted weights length for layer 1
Debug: Encrypted weights length for layer 1: 1124
secure_malloc: Allocated 1124 bytes at 0x7fe268808a00 (returned 0x7fe268808a08)
Debug: Reading encrypted weights for layer 1
Debug: Decrypting weights for layer 1 (encrypted_weights_len: 1124)
Debug: Initializing KEM
Debug: Checking key and ciphertext lengths
Debug: secret_key_len: 2400, kem->length_secret_key: 2400
Debug: ciphertext_len: 1124, kem->length_ciphertext: 1088, GCM_IV_SIZE: 12, GCM_TAG_SIZE: 16
Debug: Allocating shared secret
secure_malloc: Allocated 32 bytes at 0x6000007dd8c0 (returned 0x6000007dd8c8)
Debug: Performing KEM decapsulation
Debug: Extracting IV and tag
Debug: Creating cipher context
Debug: Initializing decryption
Debug: Setting expected tag
Debug: Allocating memory for plaintext
secure_malloc: Allocated 8 bytes at 0x600000bdc230 (returned 0x600000bdc238)
Debug: Decrypting ciphertext
Debug: Finalizing decryption
secure_free: Warning - cannot access memory before 0x6000007dd8c8
secure_free: Warning - cannot access memory before 0x7fe268808a08
Debug: Decrypted weights length for layer 1: 8
Debug: Reading public key
Debug: Public key length: 1184
secure_malloc: Allocated 1184 bytes at 0x7fe26780e400 (returned 0x7fe26780e408)
Debug: Reading public key data
Debug: Model loaded successfully
secure_malloc: Allocated 3136 bytes at 0x7fe26780ea00 (returned 0x7fe26780ea08)
Generated random input:
First 10 elements: [0.193891, 0.719871, 0.878327, 0.037810, 0.468485, 0.832346, 0.243201, 0.471933, 0.776350, 0.118360]
secure_malloc: Allocated 3136 bytes at 0x7fe26780f800 (returned 0x7fe26780f808)
secure_malloc: Allocated 1088 bytes at 0x7fe267810600 (returned 0x7fe267810608)
secure_malloc: Allocated 32 bytes at 0x6000007d8630 (returned 0x6000007d8638)
secure_malloc: Allocated 3168 bytes at 0x7fe267810c00 (returned 0x7fe267810c08)
secure_malloc: Allocated 4252 bytes at 0x7fe267811a00 (returned 0x7fe267811a08)
secure_free: Warning - cannot access memory before 0x7fe267810608
secure_free: Warning - cannot access memory before 0x6000007d8638
secure_free: Warning - cannot access memory before 0x7fe267810c08
qrme(65732,0x7ff857258dc0) malloc: *** error for object 0x7fe26780f808: pointer being freed was not allocated
qrme(65732,0x7ff857258dc0) malloc: *** set a breakpoint in malloc_error_break to debug
make: *** [run] Abort trap: 6
```
