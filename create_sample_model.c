#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oqs/oqs.h>
#include "include/model.h"
#include "include/encryption.h"
#include "include/utils.h"

#define TEST_MODEL_FILE "test_model.bin"
#define TEST_SECRET_KEY_FILE "test_secret.key"
#define INPUT_SIZE 784
#define HIDDEN_SIZE 512
#define OUTPUT_SIZE 10

int main() {
    FILE* key_file = NULL;
    Model* model = NULL;
    uint8_t *public_key = NULL, *secret_key = NULL;
    float* weights1 = NULL, *weights2 = NULL;
    size_t public_key_len, secret_key_len;
    int ret = 1;  // Default to error

    printf("Creating sample encrypted model...\n");

    // Initialize modules
    init_encryption();
    init_random();

    // Create a new model
    model = create_model();
    if (model == NULL) {
        fprintf(stderr, "Failed to create model: %s\n", get_model_error());
        goto cleanup;
    }

    // Add first layer (INPUT_SIZE -> HIDDEN_SIZE)
    weights1 = generate_random_float_array(INPUT_SIZE * HIDDEN_SIZE, -1.0f, 1.0f);
    if (weights1 == NULL) {
        fprintf(stderr, "Failed to generate weights for first layer: %s\n", get_utils_error());
        goto cleanup;
    }
    if (add_layer(model, weights1, HIDDEN_SIZE, INPUT_SIZE) != 0) {
        fprintf(stderr, "Failed to add first layer: %s\n", get_model_error());
        goto cleanup;
    }

    // Add second layer (HIDDEN_SIZE -> OUTPUT_SIZE)
    weights2 = generate_random_float_array(HIDDEN_SIZE * OUTPUT_SIZE, -1.0f, 1.0f);
    if (weights2 == NULL) {
        fprintf(stderr, "Failed to generate weights for second layer: %s\n", get_utils_error());
        goto cleanup;
    }
    if (add_layer(model, weights2, OUTPUT_SIZE, HIDDEN_SIZE) != 0) {
        fprintf(stderr, "Failed to add second layer: %s\n", get_model_error());
        goto cleanup;
    }

    // Generate a key pair for encryption
    if (generate_keypair(&public_key, &public_key_len, &secret_key, &secret_key_len) != 0) {
        fprintf(stderr, "Failed to generate key pair: %s\n", get_error());
        goto cleanup;
    }

    // Save the model
    if (save_model(model, TEST_MODEL_FILE, public_key, public_key_len) != 0) {
        fprintf(stderr, "Failed to save model: %s\n", get_model_error());
        goto cleanup;
    }

    printf("Sample encrypted model created and saved as %s\n", TEST_MODEL_FILE);
    printf("Model structure:\n");
    printf("  Input size: %d\n", INPUT_SIZE);
    printf("  Hidden layer size: %d\n", HIDDEN_SIZE);
    printf("  Output size: %d\n", OUTPUT_SIZE);

    // Save the secret key to a separate file for testing purposes
    key_file = fopen(TEST_SECRET_KEY_FILE, "wb");
    if (key_file == NULL) {
        fprintf(stderr, "Failed to create secret key file\n");
        goto cleanup;
    }

    if (fwrite(secret_key, 1, secret_key_len, key_file) != secret_key_len) {
        fprintf(stderr, "Failed to write secret key to file\n");
        fclose(key_file);
        goto cleanup;
    }

    fclose(key_file);
    printf("Secret key saved as %s (length: %zu)\n", TEST_SECRET_KEY_FILE, secret_key_len);

    ret = 0;  // Success

cleanup:
    if (weights1) secure_free((void**)&weights1);
    if (weights2) secure_free((void**)&weights2);
    if (model) free_model(model);
    if (public_key) secure_free((void**)&public_key);
    if (secret_key) secure_free((void**)&secret_key);
    cleanup_encryption();

    return ret;
}
