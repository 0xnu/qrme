#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/encryption.h"
#include "include/utils.h"
#include "include/model.h"

#define INPUT_SIZE 784  // MNIST-like input size
#define OUTPUT_SIZE 10   // 10 classes for the output

int main(int argc, char* argv[]) {
    printf("Starting QRME (Full Version)...\n");

    // Check for correct usage
    if (argc != 3) {
        printf("Usage: %s <model_file> <secret_key_file>\n", argv[0]);
        return 1;
    }

    const char* model_file = argv[1];
    const char* secret_key_file = argv[2];

    printf("Model file: %s\n", model_file);
    printf("Secret key file: %s\n", secret_key_file);

    // Initialize encryption and random number generation
    init_encryption();
    init_random();

    // Load the secret key from the file
    uint8_t* secret_key = NULL;
    size_t secret_key_len;
    FILE* key_file = fopen(secret_key_file, "rb");
    if (!key_file) {
        fprintf(stderr, "Error: Unable to open secret key file.\n");
        return 1;
    }

    // Get the file size
    fseek(key_file, 0, SEEK_END);
    secret_key_len = ftell(key_file);
    fseek(key_file, 0, SEEK_SET);

    printf("Debug: Secret key length from file size: %zu\n", secret_key_len);

    // Allocate memory for the secret key
    secret_key = secure_malloc(secret_key_len);
    if (!secret_key) {
        fprintf(stderr, "Error: Unable to allocate memory for secret key.\n");
        fclose(key_file);
        return 1;
    }

    // Read the secret key
    if (fread(secret_key, 1, secret_key_len, key_file) != secret_key_len) {
        fprintf(stderr, "Error: Unable to read secret key.\n");
        fclose(key_file);
        secure_free((void**)&secret_key);
        return 1;
    }
    fclose(key_file);
    printf("Debug: Secret key loaded successfully.\n");

    // Load the encrypted model using the secret key
    printf("Debug: Loading model...\n");
    Model* model = load_model(model_file, secret_key, secret_key_len);
    if (!model) {
        fprintf(stderr, "Error: %s\n", get_model_error());
        secure_free((void**)&secret_key);
        return 1;
    }
    printf("Debug: Model loaded successfully.\n");

    // Generate a random input for inference
    printf("Debug: Generating random input...\n");
    float* input = generate_random_float_array(INPUT_SIZE, 0.0f, 1.0f);
    if (!input) {
        fprintf(stderr, "Error: Failed to generate random input.\n");
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    printf("Generated random input:\n");
    print_float_array(input, 10, "First 10 elements");

    // Allocate memory for output array
    printf("Debug: Allocating memory for output...\n");
    float* output = (float*)secure_malloc(OUTPUT_SIZE * sizeof(float));
    if (!output) {
        fprintf(stderr, "Error: Failed to allocate memory for output.\n");
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    // Perform inference
    printf("Performing inference...\n");
    if (inference(model, input, INPUT_SIZE, output, OUTPUT_SIZE) != 0) {
        fprintf(stderr, "Error: %s\n", get_model_error());
        secure_free((void**)&output);
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    // Print inference output
    printf("Model output:\n");
    print_float_array(output, OUTPUT_SIZE, "Probabilities");

    // Determine the class with the highest probability
    float max_prob = output[0];
    int max_class = 0;
    for (int i = 1; i < OUTPUT_SIZE; i++) {
        if (output[i] > max_prob) {
            max_prob = output[i];
            max_class = i;
        }
    }

    printf("Predicted class: %d (probability: %.4f)\n", max_class, max_prob);

    // Clean up allocated resources
    printf("Cleaning up...\n");
    secure_free((void**)&output);
    secure_free((void**)&input);
    free_model(model);
    secure_free((void**)&secret_key);
    cleanup_encryption();

    printf("QRME Completed Successfully.\n");
    return 0;
}
