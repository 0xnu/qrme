# ISSUES

## 0.0.1 - 2024-09-01 - @0xnu - [Solved](./MEMORY.md)

```sh
make run                                                                                                 [6:31:57]
gcc -O3 -I. -I/usr/local/include -c src/encryption.c -o src/encryption.o
gcc -O3 -I. -I/usr/local/include -c src/model.c -o src/model.o
gcc -O3 -I. -I/usr/local/include -c src/utils.c -o src/utils.o
gcc -O3 -I. -I/usr/local/include -o qrme src/main.c src/encryption.o src/model.o src/utils.o -L/usr/local/lib -loqs -lcrypto -lm
gcc -O3 -I. -I/usr/local/include -o create_sample_model create_sample_model.c src/encryption.o src/model.o src/utils.o -L/usr/local/lib -loqs -lcrypto -lm
./create_sample_model
Creating sample encrypted model...
secure_realloc: Allocated 344 bytes at 0x7fa2e2904080 (returned 0x7fa2e2904088)
Debug: Created model at 0x7fa2e2904088
secure_realloc: Allocated 1605632 bytes at 0x7fa2e2a00000 (returned 0x7fa2e2a00008)
secure_realloc: Allocated 1605632 bytes at 0x7fa2e2b8a000 (returned 0x7fa2e2b8a008)
Debug: Added layer 0 to model at 0x7fa2e2904088, weights at 0x7fa2e2b8a008
secure_realloc: Allocated 20480 bytes at 0x7fa2e380a200 (returned 0x7fa2e380a208)
secure_realloc: Allocated 20480 bytes at 0x7fa2e380f400 (returned 0x7fa2e380f408)
Debug: Added layer 1 to model at 0x7fa2e2904088, weights at 0x7fa2e380f408
secure_realloc: Allocated 1184 bytes at 0x7fa2e3814600 (returned 0x7fa2e3814608)
secure_realloc: Allocated 2400 bytes at 0x7fa2e3814c00 (returned 0x7fa2e3814c08)
secure_realloc: Allocated 1088 bytes at 0x7fa2e4009e00 (returned 0x7fa2e4009e08)
secure_realloc: Allocated 32 bytes at 0x600001c68a20 (returned 0x600001c68a28)
secure_realloc: Allocated 1605664 bytes at 0x7fa2e4800000 (returned 0x7fa2e4800008)
secure_realloc: Allocated 1606748 bytes at 0x7fa2e4989000 (returned 0x7fa2e4989008)
secure_free: Warning - cannot access memory before 0x7fa2e4009e08
secure_free: Warning - cannot access memory before 0x600001c68a28
secure_free: Warning - cannot access memory before 0x7fa2e4800008
secure_free: Warning - cannot access memory before 0x7fa2e4989008
secure_realloc: Allocated 1088 bytes at 0x7fa2e3815600 (returned 0x7fa2e3815608)
secure_realloc: Allocated 32 bytes at 0x600001c602d0 (returned 0x600001c602d8)
secure_realloc: Allocated 20512 bytes at 0x7fa2e3816e00 (returned 0x7fa2e3816e08)
secure_realloc: Allocated 21596 bytes at 0x7fa2e381c000 (returned 0x7fa2e381c008)
secure_free: Warning - cannot access memory before 0x7fa2e3815608
secure_free: Warning - cannot access memory before 0x600001c602d8
secure_free: Warning - cannot access memory before 0x7fa2e3816e08
secure_free: Warning - cannot access memory before 0x7fa2e381c008
Sample encrypted model created and saved as test_model.bin
Model structure:
  Input size: 784
  Hidden layer size: 512
  Output size: 10
Secret key saved as test_secret.key (length: 2400)
secure_free: Warning - cannot access memory before 0x7fa2e2a00008
secure_free: Warning - cannot access memory before 0x7fa2e380a208
Debug: Freeing model at 0x7fa2e2904088
Debug: Freeing layer 0 weights at 0x7fa2e2b8a008
secure_free: Warning - cannot access memory before 0x7fa2e2b8a008
Debug: Freeing layer 1 weights at 0x7fa2e380f408
secure_free: Warning - cannot access memory before 0x7fa2e380f408
secure_free: Warning - cannot access memory before 0x7fa2e2904088
Debug: Model freed
secure_free: Warning - cannot access memory before 0x7fa2e3814608
secure_free: Warning - cannot access memory before 0x7fa2e3814c08
./qrme test_model.bin test_secret.key
secure_realloc: Allocated 2400 bytes at 0x7f963e80c400 (returned 0x7f963e80c408)
secure_realloc: Allocated 344 bytes at 0x7f963e704580 (returned 0x7f963e704588)
Debug: Created model at 0x7f963e704588
Debug: Created model at 0x7f963e704588 during load_model
Debug: Secret key length: 2400
Debug: Reading number of layers
Debug: Number of layers: 2
Debug: Reading layer 0 dimensions
Debug: Layer 0 dimensions: 512 x 784
Debug: Reading encrypted weights length for layer 0
Debug: Encrypted weights length for layer 0: 1606748
secure_realloc: Allocated 1606748 bytes at 0x7f963f000000 (returned 0x7f963f000008)
Debug: Reading encrypted weights for layer 0
Debug: Decrypting weights for layer 0 (encrypted_weights_len: 1606748)
Debug: Initializing KEM
Debug: Checking key and ciphertext lengths
Debug: secret_key_len: 2400, kem->length_secret_key: 2400
Debug: ciphertext_len: 1606748, kem->length_ciphertext: 1088, GCM_IV_SIZE: 12, GCM_TAG_SIZE: 16
Debug: Allocating shared secret
secure_realloc: Allocated 32 bytes at 0x600002fc0000 (returned 0x600002fc0008)
Debug: Performing KEM decapsulation
Debug: Extracting IV and tag
Debug: Creating cipher context
Debug: Initializing decryption
Debug: Setting expected tag
Debug: Allocating memory for plaintext
secure_realloc: Allocated 1605632 bytes at 0x7f963f300000 (returned 0x7f963f300008)
Debug: Decrypting ciphertext
Debug: Finalizing decryption
secure_free: Warning - cannot access memory before 0x600002fc0008
secure_free: Warning - cannot access memory before 0x7f963f000008
Debug: Decrypted weights length for layer 0: 1605632
Debug: Reading layer 1 dimensions
Debug: Layer 1 dimensions: 10 x 512
Debug: Reading encrypted weights length for layer 1
Debug: Encrypted weights length for layer 1: 21596
secure_realloc: Allocated 21596 bytes at 0x7f963e80de00 (returned 0x7f963e80de08)
Debug: Reading encrypted weights for layer 1
Debug: Decrypting weights for layer 1 (encrypted_weights_len: 21596)
Debug: Initializing KEM
Debug: Checking key and ciphertext lengths
Debug: secret_key_len: 2400, kem->length_secret_key: 2400
Debug: ciphertext_len: 21596, kem->length_ciphertext: 1088, GCM_IV_SIZE: 12, GCM_TAG_SIZE: 16
Debug: Allocating shared secret
secure_realloc: Allocated 32 bytes at 0x600002fc4270 (returned 0x600002fc4278)
Debug: Performing KEM decapsulation
Debug: Extracting IV and tag
Debug: Creating cipher context
Debug: Initializing decryption
Debug: Setting expected tag
Debug: Allocating memory for plaintext
secure_realloc: Allocated 20480 bytes at 0x7f963e813400 (returned 0x7f963e813408)
Debug: Decrypting ciphertext
Debug: Finalizing decryption
secure_free: Warning - cannot access memory before 0x600002fc4278
secure_free: Warning - cannot access memory before 0x7f963e80de08
Debug: Decrypted weights length for layer 1: 20480
Debug: Public key length: 1184
secure_realloc: Allocated 1184 bytes at 0x7f963e818600 (returned 0x7f963e818608)
Debug: Public key loaded successfully
Debug: Model loaded and decrypted successfully
secure_realloc: Allocated 3136 bytes at 0x7f963e818c00 (returned 0x7f963e818c08)
Generated random input:
First 10 elements: [0.807539, 0.312569, 0.339262, 0.980660, 0.950262, 0.056343, 0.949216, 0.478122, 0.788602, 0.034338]
secure_realloc: Allocated 3136 bytes at 0x7f963e819a00 (returned 0x7f963e819a08)
secure_realloc: Allocated 1088 bytes at 0x7f963e81a800 (returned 0x7f963e81a808)
secure_realloc: Allocated 32 bytes at 0x600002fc42a0 (returned 0x600002fc42a8)
secure_realloc: Allocated 3168 bytes at 0x7f963e81ae00 (returned 0x7f963e81ae08)
secure_realloc: Allocated 4252 bytes at 0x7f963e81bc00 (returned 0x7f963e81bc08)
secure_free: Warning - cannot access memory before 0x7f963e81a808
secure_free: Warning - cannot access memory before 0x600002fc42a8
secure_free: Warning - cannot access memory before 0x7f963e81ae08
secure_free: Warning - cannot access memory before 0x7f963e819a08
Debug: Initializing KEM
Debug: Checking key and ciphertext lengths
Debug: secret_key_len: 2400, kem->length_secret_key: 2400
Debug: ciphertext_len: 4252, kem->length_ciphertext: 1088, GCM_IV_SIZE: 12, GCM_TAG_SIZE: 16
Debug: Allocating shared secret
secure_realloc: Allocated 32 bytes at 0x600002fc44e0 (returned 0x600002fc44e8)
Debug: Performing KEM decapsulation
Debug: Extracting IV and tag
Debug: Creating cipher context
Debug: Initializing decryption
Debug: Setting expected tag
Debug: Allocating memory for plaintext
secure_realloc: Allocated 3136 bytes at 0x7f963e81ce00 (returned 0x7f963e81ce08)
Debug: Decrypting ciphertext
Debug: Finalizing decryption
secure_free: Warning - cannot access memory before 0x600002fc44e8
secure_free: Warning - cannot access memory before 0x7f963e81bc08
secure_realloc: Allocated 3136 bytes at 0x7f963e81dc00 (returned 0x7f963e81dc08)
secure_free: Warning - cannot access memory before 0x7f963e81ce08
secure_realloc: Allocated 40 bytes at 0x600002fc4510 (returned 0x600002fc4518)
Debug: Model public key length: 1184
Debug: Model has 2 layers
Debug: Input size: 784, Expected input size: 784
Debug: Output size: 10, Expected output size: 10
secure_realloc: Allocated 3136 bytes at 0x7f963e81ea00 (returned 0x7f963e81ea08)
secure_realloc: Allocated 40 bytes at 0x600002fc4540 (returned 0x600002fc4548)
Debug: Processing layer 0 (512 x 784)
Debug: Layer 0 output:
Layer output: [0.266065, 5.795084, 0.000000, 6.689446, 5.786364, 4.960549, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 2.853580, 6.787637, 0.000000, 17.686872, 0.000000, 0.254015, 0.000000, 0.000000, 7.466116, 0.000000, 0.000000, 0.000000, 12.189940, 0.000000, 1.193950, 11.724427, 9.229342, 0.000000, 4.386330, 0.000000, 2.502073, 1.148368, 3.206989, 11.739275, 0.000000, 0.000000, 1.131089, 2.390724, 0.000000, 0.000000, 5.523147, 0.099950, 13.131406, 2.380247, 0.000000, 17.023270, 3.417445, 0.000000, 0.000000, 9.372454, 8.227161, 0.000000, 0.000000, 0.000000, 3.477156, 9.659976, 0.000000, 0.000000, 0.000000, 0.817090, 2.975880, 0.000000, 7.767358, 22.603365, 5.982015, 2.363807, 0.000000, 3.688810, 0.000000, 0.000000, 0.000000, 2.564271, 0.000000, 3.713891, 0.000000, 0.000000, 0.000000, 3.210964, 4.477662, 11.618285, 8.740911, 9.202036, 6.919016, 6.092592, 0.000000, 7.472926, 0.000000, 0.000000, 0.000000, 3.334364, 3.171264, 0.000000, 0.000000, 0.000000, 8.758811, 0.000000, 3.947364, 0.000000, 4.402325, 11.032809, 16.011799, 2.826439, 0.000000, 0.000000, 0.000000, 0.000000, 15.829289, 9.378243, 1.676026, 0.000000, 0.000000, 0.000000, 7.829785, 1.815926, 8.007378, 0.000000, 11.308992, 0.000000, 11.096405, 7.758123, 2.063959, 9.722864, 1.373873, 0.000000, 5.236786, 9.164328, 0.000000, 0.000000, 15.422663, 9.134289, 12.116875, 0.000000, 0.000000, 0.000000, 5.661807, 0.000000, 0.000000, 0.000000, 9.547462, 11.376513, 1.384756, 0.000000, 10.384625, 0.778078, 0.000000, 0.000000, 0.000000, 15.259066, 0.000000, 1.424746, 5.706314, 0.000000, 3.380671, 8.127775, 8.086077, 4.678050, 0.000000, 0.000000, 0.000000, 5.953978, 13.628854, 0.000000, 7.457314, 0.000000, 0.114144, 7.207638, 0.000000, 0.000000, 6.187460, 11.079010, 7.716516, 14.127107, 0.000000, 0.000000, 13.138176, 0.000000, 2.404644, 2.086218, 6.635303, 0.000000, 8.089916, 14.361718, 3.726724, 0.000000, 0.000000, 10.213592, 0.000000, 0.000000, 3.858868, 6.486234, 0.000000, 8.757171, 1.786532, 6.483045, 0.000000, 0.000000, 4.364305, 0.000000, 0.000000, 14.097850, 8.338621, 1.326669, 2.902650, 11.221195, 2.339052, 0.000000, 0.000000, 0.000000, 7.314142, 6.059758, 9.642334, 12.248257, 3.401198, 18.088888, 4.749819, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.777178, 0.000000, 12.383888, 2.531278, 0.000000, 1.505692, 4.923979, 0.000000, 11.425014, 0.000000, 0.000000, 0.000000, 10.933193, 0.000000, 2.023014, 4.632501, 0.000000, 3.464091, 0.299572, 0.000000, 6.685752, 16.864620, 4.324139, 1.124085, 0.000000, 0.000000, 0.000000, 0.000000, 5.241763, 20.741795, 7.216500, 0.000000, 0.000000, 0.000000, 0.000000, 9.135165, 0.000000, 0.000000, 11.928494, 0.186247, 0.000000, 3.814284, 0.000000, 0.000000, 1.430919, 14.556467, 0.000000, 19.612669, 5.241525, 5.953288, 2.288954, 0.000000, 0.000000, 5.994075, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 9.340139, 9.203889, 0.000000, 1.137733, 6.405480, 0.000000, 2.378010, 4.292865, 9.207808, 7.016627, 0.000000, 0.000000, 0.000000, 0.000000, 10.199704, 14.998979, 5.476542, 0.000000, 20.039963, 1.867344, 4.310074, 10.363213, 7.255196, 5.609289, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 5.487425, 3.794948, 0.000000, 0.000000, 0.000000, 2.113547, 0.000000, 0.000000, 3.097557, 0.000000, 0.000000, 0.000000, 9.256351, 0.000000, 5.258914, 0.000000, 0.000000, 8.724354, 0.000000, 8.823584, 0.001339, 0.000000, 0.000000, 19.004210, 0.000000, 3.727042, 2.058402, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 7.962681, 0.000000, 10.567538, 0.000000, 0.000000, 8.184501, 25.439966, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 17.832495, 0.000000, 0.000000, 5.885395, 4.768107, 3.642365, 2.064259, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 10.812338, 0.000000, 0.000000, 0.000000, 0.000000, 3.628180, 0.000000, 0.000000, 0.000000, 0.000000, 11.618003, 0.159778, 6.384268, 0.000000, 0.000000, 0.000000, 14.558215, 13.201638, 6.745589, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 6.061272, 0.000000, 2.911002, 0.000000, 1.585483, 2.841456, 0.000000, 12.599723, 6.333182, 8.396608, 0.000000, 3.924665, 0.000000, 10.542898, 0.000000, 12.615968, 0.000000, 17.139383, 1.090970, 1.220914, 0.000000, 12.587090, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 9.660908, 4.580121, 0.000000, 8.843564, 0.000000, 11.154247, 3.859651, 15.074762, 9.444498, 0.000000, 4.203628, 0.000000, 0.000000, 0.000000, 0.000000, 1.079097, 1.063756, 0.953007, 5.260622, 0.000000, 0.000000, 11.596365, 3.265605, 0.000000, 0.000000, 0.000000, 10.094927, 14.216016, 0.000000, 0.000000, 23.113951, 0.000000, 0.000000, 6.721364, 0.000000, 0.000000, 0.000000, 3.252408, 1.874552, 0.000000, 0.303172, 7.003495, 0.000000, 3.650027, 0.000000, 0.000000, 15.100171, 0.000000, 0.000000, 0.000000, 9.103717, 3.596967, 0.000000, 11.985738, 1.890219, 9.467557, 0.000000, 1.722360, 4.215482, 9.419692, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 8.908494, 0.000000, 7.188535, 5.256529, 0.000000, 0.000000, 2.644534, 0.640109, 1.282604, 0.000000, 0.443796, 0.000000, 0.000000, 0.000000, 13.644991, 0.000000, 5.683582, 0.000000, 5.459587, 23.185099]
Debug: Processing layer 1 (10 x 512)
Debug: Layer 1 output:
Layer output: [82.174492, 0.000000, 6.676238, 121.179192, 0.000000, 3.518883, 0.000000, 20.347832, 141.769547, 0.000000]
secure_free: Warning - cannot access memory before 0x7f963e81ea08
secure_free: Warning - cannot access memory before 0x600002fc4548
secure_free: Warning - cannot access memory before 0x7f963e81dc08
Model output:
Probabilities: [82.174492, 0.000000, 6.676238, 121.179192, 0.000000, 3.518883, 0.000000, 20.347832, 141.769547, 0.000000]
Predicted class: 8 (probability: 141.7695)
secure_realloc: Allocated 40 bytes at 0x600002fc4570 (returned 0x600002fc4578)
secure_realloc: Allocated 1088 bytes at 0x7f963e81f800 (returned 0x7f963e81f808)
secure_realloc: Allocated 32 bytes at 0x600002fc45a0 (returned 0x600002fc45a8)
secure_realloc: Allocated 72 bytes at 0x6000002c40f0 (returned 0x6000002c40f8)
secure_realloc: Allocated 1156 bytes at 0x7f963e81fe00 (returned 0x7f963e81fe08)
secure_free: Warning - cannot access memory before 0x7f963e81f808
secure_free: Warning - cannot access memory before 0x600002fc45a8
secure_free: Warning - cannot access memory before 0x6000002c40f8
secure_free: Warning - cannot access memory before 0x600002fc4578
Encrypted output length: 1156 bytes
secure_free: Warning - cannot access memory before 0x7f963e81fe08
secure_free: Warning - cannot access memory before 0x600002fc4518
secure_free: Warning - cannot access memory before 0x7f963e818c08
Debug: Freeing model at 0x7f963e704588
Debug: Freeing layer 0 weights at 0x7f963f300008
secure_free: Warning - cannot access memory before 0x7f963f300008
Debug: Freeing layer 1 weights at 0x7f963e813408
secure_free: Warning - cannot access memory before 0x7f963e813408
Debug: Freeing public key at 0x7f963e818608
secure_free: Warning - cannot access memory before 0x7f963e818608
secure_free: Warning - cannot access memory before 0x7f963e704588
Debug: Model freed
secure_free: Warning - cannot access memory before 0x7f963e80c408
Quantum-resistant encrypted inference completed successfully.
```

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
