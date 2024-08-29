#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "../include/encryption.h"
#include "../include/model.h"
#include "../include/utils.h"

#define TEST_MESSAGE "Hello, LLM and Quantum World!"
#define EPSILON 1e-6
#define TEST_MODEL_FILE "test_model.bin"

// Helper function to compare float arrays
int compare_float_arrays(const float* a, const float* b, size_t len, float epsilon) {
    for (size_t i = 0; i < len; i++) {
        if (fabs(a[i] - b[i]) > epsilon) {
            return 0;
        }
    }
    return 1;
}

// Encryption Tests
void test_key_generation() {
    printf("Testing key generation...\n");

    uint8_t *public_key, *secret_key;
    size_t public_key_len, secret_key_len;

    int result = generate_keypair(&public_key, &public_key_len, &secret_key, &secret_key_len);
    assert(result == 0);
    assert(public_key != NULL);
    assert(secret_key != NULL);
    assert(public_key_len > 0);
    assert(secret_key_len > 0);

    printf("Public key length: %zu\n", public_key_len);
    printf("Secret key length: %zu\n", secret_key_len);

    cleanup(public_key);
    cleanup(secret_key);

    printf("Key generation test passed.\n\n");
}

void test_encryption_decryption() {
    printf("Testing encryption and decryption...\n");

    uint8_t *public_key, *secret_key;
    size_t public_key_len, secret_key_len;

    int result = generate_keypair(&public_key, &public_key_len, &secret_key, &secret_key_len);
    assert(result == 0);

    const uint8_t *plaintext = (const uint8_t *)TEST_MESSAGE;
    size_t plaintext_len = strlen(TEST_MESSAGE);

    uint8_t *ciphertext;
    size_t ciphertext_len;

    result = encrypt(public_key, public_key_len, plaintext, plaintext_len, &ciphertext, &ciphertext_len);
    assert(result == 0);
    assert(ciphertext != NULL);
    assert(ciphertext_len > plaintext_len);

    uint8_t *decrypted;
    size_t decrypted_len;

    result = decrypt(secret_key, secret_key_len, ciphertext, ciphertext_len, &decrypted, &decrypted_len);
    assert(result == 0);
    assert(decrypted != NULL);
    assert(decrypted_len == plaintext_len);
    assert(memcmp(plaintext, decrypted, plaintext_len) == 0);

    printf("Original message: %s\n", TEST_MESSAGE);
    printf("Decrypted message: %s\n", decrypted);

    cleanup(public_key);
    cleanup(secret_key);
    cleanup(ciphertext);
    cleanup(decrypted);

    printf("Encryption and decryption test passed.\n\n");
}

// Model Tests
void test_create_model() {
    printf("Testing model creation...\n");

    Model* model = create_model();
    assert(model != NULL);

    free_model(model);

    printf("Model creation test passed.\n\n");
}

void test_add_layer() {
    printf("Testing add layer...\n");

    Model* model = create_model();
    assert(model != NULL);

    float weights[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    int result = add_layer(model, weights, 2, 3);
    assert(result == 0);

    free_model(model);

    printf("Add layer test passed.\n\n");
}

void test_save_load_model() {
    printf("Testing save and load model...\n");

    // Create and populate a model
    Model* model = create_model();
    assert(model != NULL);

    float weights1[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    float weights2[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    add_layer(model, weights1, 2, 3);
    add_layer(model, weights2, 5, 1);

    // Generate a key pair for encryption
    uint8_t *public_key, *secret_key;
    size_t public_key_len, secret_key_len;
    int result = generate_keypair(&public_key, &public_key_len, &secret_key, &secret_key_len);
    assert(result == 0);

    // Save the model
    result = save_model(model, TEST_MODEL_FILE, public_key, public_key_len);
    assert(result == 0);

    // Free the original model
    free_model(model);

    // Load the model
    Model* loaded_model = load_model(TEST_MODEL_FILE, secret_key, secret_key_len);
    assert(loaded_model != NULL);

    // Clean up
    free_model(loaded_model);
    cleanup(public_key);
    cleanup(secret_key);
    remove(TEST_MODEL_FILE);

    printf("Save and load model test passed.\n\n");
}

void test_inference() {
    printf("Testing model inference...\n");

    // Create and populate a model
    Model* model = create_model();
    assert(model != NULL);

    // Simple model: 2 layers, 3 inputs, 2 hidden neurons, 1 output
    float weights1[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
    float weights2[] = {0.7f, 0.8f};
    add_layer(model, weights1, 2, 3);
    add_layer(model, weights2, 1, 2);

    // Prepare input
    float input[] = {1.0f, 2.0f, 3.0f};
    float output[1];

    // Perform inference
    int result = inference(model, input, 3, output, 1);
    assert(result == 0);

    // Expected output: ReLU(0.7 * ReLU(0.1*1 + 0.2*2 + 0.3*3) + 0.8 * ReLU(0.4*1 + 0.5*2 + 0.6*3))
    // = ReLU(0.7 * ReLU(1.4) + 0.8 * ReLU(3.2))
    // = ReLU(0.7 * 1.4 + 0.8 * 3.2)
    // = ReLU(0.98 + 2.56)
    // = 3.54
    float expected_output = 3.54f;
    assert(fabs(output[0] - expected_output) < EPSILON);

    // Clean up
    free_model(model);

    printf("Model inference test passed.\n\n");
}

int main() {
    printf("Starting all tests...\n\n");

    init_encryption();
    init_random();

    // Encryption tests
    test_key_generation();
    test_encryption_decryption();

    // Model tests
    test_create_model();
    test_add_layer();
    test_save_load_model();
    test_inference();

    cleanup_encryption();

    printf("All tests passed successfully!\n");

    return 0;
}
