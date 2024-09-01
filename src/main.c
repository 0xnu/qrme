#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/encryption.h"
#include "../include/model.h"
#include "../include/utils.h"

#define INPUT_SIZE 784  // MNIST-like input size
#define OUTPUT_SIZE 10  // 10 classes for classification

void print_usage(const char* program_name) {
    printf("Usage: %s <model_file> <secret_key_file>\n", program_name);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char* model_file = argv[1];
    const char* secret_key_file = argv[2];

    // Initialize the encryption and utility modules
    init_encryption();
    init_random();

    // Load the secret key
    uint8_t* secret_key;
    size_t secret_key_len;
    FILE* key_file = fopen(secret_key_file, "rb");
    if (!key_file) {
        fprintf(stderr, "Error: Unable to open secret key file.\n");
        return 1;
    }
    fseek(key_file, 0, SEEK_END);
    secret_key_len = ftell(key_file);
    fseek(key_file, 0, SEEK_SET);
    secret_key = secure_realloc(NULL, secret_key_len);
    if (!secret_key || fread(secret_key, 1, secret_key_len, key_file) != secret_key_len) {
        fprintf(stderr, "Error: Unable to read secret key.\n");
        fclose(key_file);
        return 1;
    }
    fclose(key_file);

    // Load the encrypted model
    Model* model = load_model(model_file, secret_key, secret_key_len);
    if (!model) {
        fprintf(stderr, "Error: %s\n", get_model_error());
        secure_free((void**)&secret_key);
        return 1;
    }

    // Get the model's public key
    const uint8_t* public_key;
    size_t public_key_len;
    if (get_model_public_key(model, &public_key, &public_key_len) != 0) {
        fprintf(stderr, "Error: Unable to get model's public key.\n");
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    // Generate a random input (simulating an image)
    float* input = generate_random_float_array(INPUT_SIZE, 0.0f, 1.0f);
    if (!input) {
        fprintf(stderr, "Error: %s\n", get_utils_error());
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    printf("Generated random input:\n");
    print_float_array(input, 10, "First 10 elements");  // Print first 10 elements for conciseness

    // Encrypt the input
    uint8_t* input_bytes;
    size_t input_bytes_len;
    if (float_to_byte_array(input, INPUT_SIZE, &input_bytes, &input_bytes_len) != 0) {
        fprintf(stderr, "Error: %s\n", get_utils_error());
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    uint8_t* encrypted_input;
    size_t encrypted_input_len;
    if (encrypt(public_key, public_key_len, input_bytes, input_bytes_len, &encrypted_input, &encrypted_input_len) != 0) {
        fprintf(stderr, "Error: %s\n", get_error());
        secure_free((void**)&input_bytes);
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    secure_free((void**)&input_bytes);

    // Decrypt the input (simulating what would happen on the server)
    uint8_t* decrypted_input;
    size_t decrypted_input_len;
    if (decrypt(secret_key, secret_key_len, encrypted_input, encrypted_input_len, &decrypted_input, &decrypted_input_len) != 0) {
        fprintf(stderr, "Error: %s\n", get_error());
        secure_free((void**)&encrypted_input);
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    secure_free((void**)&encrypted_input);

    // Convert decrypted input back to float array
    float* decrypted_input_float;
    size_t decrypted_input_float_len;
    if (byte_to_float_array(decrypted_input, decrypted_input_len, &decrypted_input_float, &decrypted_input_float_len) != 0) {
        fprintf(stderr, "Error: %s\n", get_utils_error());
        secure_free((void**)&decrypted_input);
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    secure_free((void**)&decrypted_input);

    // Perform inference
    float* output = secure_realloc(NULL, OUTPUT_SIZE * sizeof(float));
    if (!output) {
        fprintf(stderr, "Error: Unable to allocate memory for output.\n");
        secure_free((void**)&decrypted_input_float);
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    if (inference(model, decrypted_input_float, INPUT_SIZE, output, OUTPUT_SIZE) != 0) {
        fprintf(stderr, "Error: %s\n", get_model_error());
        secure_free((void**)&output);
        secure_free((void**)&decrypted_input_float);
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    secure_free((void**)&decrypted_input_float);

    // Print the output
    printf("Model output:\n");
    print_float_array(output, OUTPUT_SIZE, "Probabilities");

    // Find the highest probability class
    float max_prob = output[0];
    int max_class = 0;
    for (int i = 1; i < OUTPUT_SIZE; i++) {
        if (output[i] > max_prob) {
            max_prob = output[i];
            max_class = i;
        }
    }

    printf("Predicted class: %d (probability: %.4f)\n", max_class, max_prob);

    // Encrypt the output (simulating sending the result back to the client)
    uint8_t* output_bytes;
    size_t output_bytes_len;
    if (float_to_byte_array(output, OUTPUT_SIZE, &output_bytes, &output_bytes_len) != 0) {
        fprintf(stderr, "Error: %s\n", get_utils_error());
        secure_free((void**)&output);
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    uint8_t* encrypted_output;
    size_t encrypted_output_len;
    if (encrypt(public_key, public_key_len, output_bytes, output_bytes_len, &encrypted_output, &encrypted_output_len) != 0) {
        fprintf(stderr, "Error: %s\n", get_error());
        secure_free((void**)&output_bytes);
        secure_free((void**)&output);
        secure_free((void**)&input);
        free_model(model);
        secure_free((void**)&secret_key);
        return 1;
    }

    secure_free((void**)&output_bytes);

    printf("Encrypted output length: %zu bytes\n", encrypted_output_len);

    // Clean up
    secure_free((void**)&encrypted_output);
    secure_free((void**)&output);
    secure_free((void**)&input);
    free_model(model);
    secure_free((void**)&secret_key);

    // Clean up the encryption and utility modules
    cleanup_encryption();

    printf("Quantum-resistant encrypted inference completed successfully.\n");

    return 0;
}
