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

typedef void (*TestFunction)(void);

// Helper function to compare float arrays
static int compare_float_arrays(const float* a, const float* b, size_t len, float epsilon) {
    for (size_t i = 0; i < len; i++) {
        if (fabs(a[i] - b[i]) > epsilon) return 0;
    }
    return 1;
}

// Test functions
static void test_key_generation(void) {
    uint8_t *public_key = NULL, *secret_key = NULL;
    size_t public_key_len, secret_key_len;

    assert(generate_keypair(&public_key, &public_key_len, &secret_key, &secret_key_len) == 0);
    assert(public_key && secret_key && public_key_len > 0 && secret_key_len > 0);

    printf("Public key length: %zu\nSecret key length: %zu\n", public_key_len, secret_key_len);

    cleanup((void**)&public_key);
    cleanup((void**)&secret_key);
}

static void test_encryption_decryption(void) {
    uint8_t *public_key = NULL, *secret_key = NULL, *ciphertext = NULL, *decrypted = NULL;
    size_t public_key_len, secret_key_len, ciphertext_len, decrypted_len;
    const uint8_t *plaintext = (const uint8_t *)TEST_MESSAGE;
    size_t plaintext_len = strlen(TEST_MESSAGE);

    assert(generate_keypair(&public_key, &public_key_len, &secret_key, &secret_key_len) == 0);
    assert(encrypt(public_key, public_key_len, plaintext, plaintext_len, &ciphertext, &ciphertext_len) == 0);
    assert(decrypt(secret_key, secret_key_len, ciphertext, ciphertext_len, &decrypted, &decrypted_len) == 0);

    assert(decrypted_len == plaintext_len && memcmp(plaintext, decrypted, plaintext_len) == 0);

    printf("Original message: %s\nDecrypted message: %s\n", TEST_MESSAGE, decrypted);

    cleanup((void**)&public_key);
    cleanup((void**)&secret_key);
    cleanup((void**)&ciphertext);
    cleanup((void**)&decrypted);
}

static void test_create_model(void) {
    Model* model = create_model();
    assert(model != NULL);
    free_model(model);
}

static void test_add_layer(void) {
    Model* model = create_model();
    float weights[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    assert(add_layer(model, weights, 2, 3) == 0);
    free_model(model);
}

static void test_save_load_model(void) {
    Model* model = create_model();
    uint8_t *public_key = NULL, *secret_key = NULL;
    size_t public_key_len, secret_key_len;
    float weights1[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    float weights2[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};

    add_layer(model, weights1, 2, 3);
    add_layer(model, weights2, 5, 1);

    assert(generate_keypair(&public_key, &public_key_len, &secret_key, &secret_key_len) == 0);
    assert(save_model(model, TEST_MODEL_FILE, public_key, public_key_len) == 0);

    free_model(model);

    Model* loaded_model = load_model(TEST_MODEL_FILE, secret_key, secret_key_len);
    assert(loaded_model != NULL);

    free_model(loaded_model);
    cleanup((void**)&public_key);
    cleanup((void**)&secret_key);
    remove(TEST_MODEL_FILE);
}

static void test_inference(void) {
    Model* model = create_model();
    float weights1[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
    float weights2[] = {0.7f, 0.8f};
    float input[] = {1.0f, 2.0f, 3.0f};
    float output[1];
    float expected_output = 3.54f;

    add_layer(model, weights1, 2, 3);
    add_layer(model, weights2, 1, 2);

    assert(inference(model, input, 3, output, 1) == 0);
    assert(fabs(output[0] - expected_output) < EPSILON);

    free_model(model);
}

// Test runner
static void run_test(const char* test_name, TestFunction test_func) {
    printf("Testing %s...\n", test_name);
    test_func();
    printf("%s test passed.\n\n", test_name);
}

int main(void) {
    printf("Starting all tests...\n\n");

    init_encryption();
    init_random();

    TestFunction tests[] = {
        test_key_generation,
        test_encryption_decryption,
        test_create_model,
        test_add_layer,
        test_save_load_model,
        test_inference
    };

    const char* test_names[] = {
        "key generation",
        "encryption and decryption",
        "model creation",
        "add layer",
        "save and load model",
        "model inference"
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        run_test(test_names[i], tests[i]);
    }

    cleanup_encryption();

    printf("All tests passed successfully!\n");

    return 0;
}
