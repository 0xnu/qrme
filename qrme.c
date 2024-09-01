#include <stdio.h>
#include <stdlib.h>
#include "include/encryption.h"
#include "include/utils.h"
#include "include/model.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <model_file> <secret_key_file>\n", argv[0]);
        return 1;
    }

    const char* model_file = argv[1];
    const char* secret_key_file = argv[2];
    Model* model = NULL;
    uint8_t* secret_key = NULL;
    size_t secret_key_len;
    FILE* key_file = NULL;
    float* input = NULL;
    float* output = NULL;
    int ret = 1;  // Default to error

    printf("Initializing encryption...\n");
    init_encryption();
    init_random();  // Initialize random number generator

    key_file = fopen(secret_key_file, "rb");
    if (!key_file) {
        fprintf(stderr, "Error: Unable to open secret key file.\n");
        goto cleanup;
    }

    fseek(key_file, 0, SEEK_END);
    secret_key_len = ftell(key_file);
    fseek(key_file, 0, SEEK_SET);

    printf("Allocating memory for secret key (%zu bytes)...\n", secret_key_len);
    secret_key = secure_realloc(NULL, secret_key_len);
    if (!secret_key) {
        fprintf(stderr, "Error: Unable to allocate memory for secret key.\n");
        goto cleanup;
    }

    printf("Reading secret key...\n");
    if (fread(secret_key, 1, secret_key_len, key_file) != secret_key_len) {
        fprintf(stderr, "Error: Unable to read secret key.\n");
        goto cleanup;
    }

    printf("Loading and decrypting model...\n");
    model = load_model(model_file, secret_key, secret_key_len);
    if (!model) {
        fprintf(stderr, "Error: %s\n", get_model_error());
        goto cleanup;
    }

    printf("Model loaded and decrypted successfully.\n");

    // Get the input size from the first layer of the model
    size_t input_size = model->layers[0].cols;
    size_t output_size = model->layers[model->num_layers - 1].rows;

    printf("Model structure:\n");
    printf("  Number of layers: %zu\n", model->num_layers);
    for (size_t i = 0; i < model->num_layers; i++) {
        printf("  Layer %zu: %zu x %zu\n", i, model->layers[i].rows, model->layers[i].cols);
    }
    printf("Model input size: %zu\n", input_size);
    printf("Model output size: %zu\n", output_size);

    // Generate random input for testing
    input = generate_random_float_array(input_size, -1.0f, 1.0f);
    if (!input) {
        fprintf(stderr, "Error: Unable to generate random input.\n");
        goto cleanup;
    }

    printf("Generated random input:\n");
    print_float_array(input, input_size, "Input");

    // Allocate memory for output
    output = secure_realloc(NULL, output_size * sizeof(float));
    if (!output) {
        fprintf(stderr, "Error: Unable to allocate memory for output.\n");
        goto cleanup;
    }

    // Perform inference
    printf("Performing inference...\n");
    if (inference(model, input, input_size, output, output_size) != 0) {
        fprintf(stderr, "Error: Failed to perform inference. %s\n", get_model_error());
        goto cleanup;
    }

    printf("Inference result:\n");
    print_float_array(output, output_size, "Output");

    ret = 0;  // Success

cleanup:
    if (key_file) fclose(key_file);
    if (secret_key) secure_free((void**)&secret_key);
    if (input) secure_free((void**)&input);
    if (output) secure_free((void**)&output);
    if (model) free_model(model);
    cleanup_encryption();

    return ret;
}
