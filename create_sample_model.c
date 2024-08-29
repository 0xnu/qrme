#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oqs/oqs.h>
#include "include/model.h"
#include "include/encryption.h"
#include "include/utils.h"

#define TEST_MODEL_FILE "test_model.bin"
#define TEST_SECRET_KEY_FILE "test_secret.key"
#define INPUT_SIZE 3
#define HIDDEN_SIZE 2
#define OUTPUT_SIZE 1

int main() {
    printf("Creating sample encrypted model...\n");

    // Initialize modules
    init_encryption();
    init_random();

    // Create a new model
    Model* model = create_model();
    printf("Model created at %p\n", (void*)model);
    if (model == NULL) {
        fprintf(stderr, "Failed to create model: %s\n", get_model_error());
        return 1;
    }

    // Add first layer (INPUT_SIZE -> HIDDEN_SIZE)
    float* weights1 = generate_random_float_array(INPUT_SIZE * HIDDEN_SIZE, -1.0f, 1.0f);
    printf("Weights1 allocated at %p\n", (void*)weights1);
    if (weights1 == NULL) {
        fprintf(stderr, "Failed to generate weights for first layer: %s\n", get_utils_error());
        free_model(model);
        return 1;
    }

    if (add_layer(model, weights1, HIDDEN_SIZE, INPUT_SIZE) != 0) {
        fprintf(stderr, "Failed to add first layer: %s\n", get_model_error());
        free(weights1);
        free_model(model);
        return 1;
    }
    // Do not free weights1 here, as add_layer has taken ownership of it

    // Add second layer (HIDDEN_SIZE -> OUTPUT_SIZE)
    float* weights2 = generate_random_float_array(HIDDEN_SIZE * OUTPUT_SIZE, -1.0f, 1.0f);
    printf("Weights2 allocated at %p\n", (void*)weights2);
    if (weights2 == NULL) {
        fprintf(stderr, "Failed to generate weights for second layer: %s\n", get_utils_error());
        free_model(model);
        return 1;
    }

    if (add_layer(model, weights2, OUTPUT_SIZE, HIDDEN_SIZE) != 0) {
        fprintf(stderr, "Failed to add second layer: %s\n", get_model_error());
        free(weights2);
        free_model(model);
        return 1;
    }
    // Do not free weights2 here, as add_layer has taken ownership of it

    // Generate a key pair for encryption
    uint8_t *public_key, *secret_key;
    size_t public_key_len, secret_key_len;
    if (generate_keypair(&public_key, &public_key_len, &secret_key, &secret_key_len) != 0) {
        fprintf(stderr, "Failed to generate key pair: %s\n", get_error());
        free_model(model);
        return 1;
    }
    printf("Public key allocated at %p\n", (void*)public_key);
    printf("Secret key allocated at %p\n", (void*)secret_key);

    // Save the model
    if (save_model(model, TEST_MODEL_FILE, public_key, public_key_len) != 0) {
        fprintf(stderr, "Failed to save model: %s\n", get_model_error());
        free_model(model);
        cleanup(public_key);
        cleanup(secret_key);
        return 1;
    }

    printf("Sample encrypted model created and saved as %s\n", TEST_MODEL_FILE);
    printf("Model structure:\n");
    printf("  Input size: %d\n", INPUT_SIZE);
    printf("  Hidden layer size: %d\n", HIDDEN_SIZE);
    printf("  Output size: %d\n", OUTPUT_SIZE);

    // Save the secret key to a separate file for testing purposes
    FILE* key_file = fopen(TEST_SECRET_KEY_FILE, "wb");
    if (key_file == NULL) {
        fprintf(stderr, "Failed to create secret key file\n");
    } else {
        // Save only the actual key length, not including any potential padding
        OQS_KEM *temp_kem = OQS_KEM_new(OQS_KEM_alg_kyber_768);
        if (temp_kem == NULL) {
            fprintf(stderr, "Failed to create KEM instance\n");
            fclose(key_file);
        } else {
            size_t actual_key_len = temp_kem->length_secret_key;
            fwrite(secret_key, 1, actual_key_len, key_file);
            fclose(key_file);
            printf("Secret key saved as %s (length: %zu)\n", TEST_SECRET_KEY_FILE, actual_key_len);
            OQS_KEM_free(temp_kem);
        }
    }

    // Test loading and inference
        Model* loaded_model = load_model(TEST_MODEL_FILE, secret_key, secret_key_len);
        printf("Loaded model at %p\n", (void*)loaded_model);
        if (loaded_model == NULL) {
            fprintf(stderr, "Failed to load model: %s\n", get_model_error());
        } else {
            printf("Successfully loaded the model.\n");

            // Perform a test inference
            float test_input[INPUT_SIZE] = {0.5f, -0.3f, 0.8f};
            float test_output[OUTPUT_SIZE];

            if (inference(loaded_model, test_input, INPUT_SIZE, test_output, OUTPUT_SIZE) == 0) {
                printf("Test inference result: %f\n", test_output[0]);
            } else {
                fprintf(stderr, "Failed to perform inference: %s\n", get_model_error());
            }

            printf("Freeing loaded model at %p\n", (void*)loaded_model);
            free_model(loaded_model);
        }

        printf("Starting cleanup...\n");

        // Clean up
        printf("Freeing original model at %p\n", (void*)model);
        free_model(model);
        printf("Cleaning up public key at %p\n", (void*)public_key);
        cleanup(public_key);
        printf("Cleaning up secret key at %p\n", (void*)secret_key);
        cleanup(secret_key);
        printf("Cleaning up encryption...\n");
        cleanup_encryption();

        printf("Cleanup complete.\n");

        return 0;
}
