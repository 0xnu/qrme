#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/model.h"
#include "../include/encryption.h"
#include "../include/utils.h"

#define MAX_ERROR_LENGTH 256

static char error_message[MAX_ERROR_LENGTH] = {0};

static void set_error(const char* message) {
    strncpy(error_message, message, MAX_ERROR_LENGTH - 1);
    error_message[MAX_ERROR_LENGTH - 1] = '\0';
}

const char* get_model_error(void) {
    return error_message;
}

Model* create_model(void) {
    Model* model = secure_malloc(sizeof(Model));
    if (!model) {
        set_error("Failed to allocate memory for model");
        return NULL;
    }
    memset(model, 0, sizeof(Model));
    return model;
}

int add_layer(Model* model, const float* weights, size_t rows, size_t cols) {
    if (!model) {
        set_error("Invalid model pointer");
        return -1;
    }
    if (model->num_layers >= MAX_LAYERS) {
        set_error("Maximum number of layers reached");
        return -1;
    }

    Layer* layer = &model->layers[model->num_layers];
    layer->weights = secure_malloc(rows * cols * sizeof(float));
    if (!layer->weights) {
        set_error("Failed to allocate memory for layer weights");
        return -1;
    }

    memcpy(layer->weights, weights, rows * cols * sizeof(float));
    layer->rows = rows;
    layer->cols = cols;
    layer->is_secure_allocated = 1;
    model->num_layers++;

    return 0;
}

int save_model(const Model* model, const char* filename, const uint8_t* public_key, size_t public_key_len) {
    if (!model || !filename || !public_key) {
        set_error("Invalid parameters for save_model");
        return -1;
    }

    FILE* file = fopen(filename, "wb");
    if (!file) {
        set_error("Failed to open file for writing");
        return -1;
    }

    // Write number of layers
    fwrite(&model->num_layers, sizeof(size_t), 1, file);

    // Write each layer
    for (size_t i = 0; i < model->num_layers; i++) {
        const Layer* layer = &model->layers[i];
        fwrite(&layer->rows, sizeof(size_t), 1, file);
        fwrite(&layer->cols, sizeof(size_t), 1, file);

        // Encrypt weights
        uint8_t* encrypted_weights;
        size_t encrypted_weights_len;
        if (encrypt(public_key, public_key_len, (uint8_t*)layer->weights,
                    layer->rows * layer->cols * sizeof(float),
                    &encrypted_weights, &encrypted_weights_len) != 0) {
            set_error("Failed to encrypt layer weights");
            fclose(file);
            return -1;
        }

        // Write encrypted weights
        fwrite(&encrypted_weights_len, sizeof(size_t), 1, file);
        fwrite(encrypted_weights, 1, encrypted_weights_len, file);
        secure_free((void**)&encrypted_weights);
    }

    // Write public key
    fwrite(&public_key_len, sizeof(size_t), 1, file);
    fwrite(public_key, 1, public_key_len, file);

    fclose(file);
    return 0;
}

Model* load_model(const char* filename, const uint8_t* secret_key, size_t secret_key_len) {
    if (!filename || !secret_key) {
        set_error("Invalid parameters for load_model");
        return NULL;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        set_error("Failed to open file for reading");
        return NULL;
    }

    Model* model = create_model();
    if (!model) {
        fclose(file);
        return NULL;
    }

    // Read number of layers
    if (fread(&model->num_layers, sizeof(size_t), 1, file) != 1) {
        set_error("Failed to read number of layers");
        free_model(model);
        fclose(file);
        return NULL;
    }

    // Read each layer
    for (size_t i = 0; i < model->num_layers; i++) {
        Layer* layer = &model->layers[i];
        if (fread(&layer->rows, sizeof(size_t), 1, file) != 1 ||
            fread(&layer->cols, sizeof(size_t), 1, file) != 1) {
            set_error("Failed to read layer dimensions");
            free_model(model);
            fclose(file);
            return NULL;
        }

        // Read encrypted weights
        size_t encrypted_weights_len;
        if (fread(&encrypted_weights_len, sizeof(size_t), 1, file) != 1) {
            set_error("Failed to read encrypted weights length");
            free_model(model);
            fclose(file);
            return NULL;
        }

        uint8_t* encrypted_weights = secure_malloc(encrypted_weights_len);
        if (!encrypted_weights) {
            set_error("Failed to allocate memory for encrypted weights");
            free_model(model);
            fclose(file);
            return NULL;
        }

        if (fread(encrypted_weights, 1, encrypted_weights_len, file) != encrypted_weights_len) {
            set_error("Failed to read encrypted weights");
            secure_free((void**)&encrypted_weights);
            free_model(model);
            fclose(file);
            return NULL;
        }

        // Decrypt weights
        uint8_t* decrypted_weights;
        size_t decrypted_weights_len;
        if (decrypt(secret_key, secret_key_len, encrypted_weights, encrypted_weights_len,
                    &decrypted_weights, &decrypted_weights_len) != 0) {
            set_error("Failed to decrypt layer weights");
            secure_free((void**)&encrypted_weights);
            free_model(model);
            fclose(file);
            return NULL;
        }

        secure_free((void**)&encrypted_weights);

        layer->weights = (float*)decrypted_weights;
        layer->is_secure_allocated = 1;  // Mark as secure allocated
        if (decrypted_weights_len != layer->rows * layer->cols * sizeof(float)) {
            set_error("Decrypted weights size mismatch");
            free_model(model);
            fclose(file);
            return NULL;
        }
    }

    // Read public key
    if (fread(&model->public_key_len, sizeof(size_t), 1, file) != 1) {
        set_error("Failed to read public key length");
        free_model(model);
        fclose(file);
        return NULL;
    }

    model->public_key = secure_malloc(model->public_key_len);
    if (!model->public_key) {
        set_error("Failed to allocate memory for public key");
        free_model(model);
        fclose(file);
        return NULL;
    }

    if (fread(model->public_key, 1, model->public_key_len, file) != model->public_key_len) {
        set_error("Failed to read public key");
        free_model(model);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return model;
}

int inference(const Model* model, const float* input, size_t input_size,
              float* output, size_t output_size) {
    if (!model || !input || !output) {
        set_error("Invalid parameters for inference");
        return -1;
    }

    if (model->num_layers == 0) {
        set_error("Model has no layers");
        return -1;
    }

    if (input_size != model->layers[0].cols) {
        set_error("Input size mismatch");
        return -1;
    }

    if (output_size != model->layers[model->num_layers - 1].rows) {
        set_error("Output size mismatch");
        return -1;
    }

    float* temp_input = (float*)secure_malloc(input_size * sizeof(float));
    float* temp_output = (float*)secure_malloc(output_size * sizeof(float));
    if (!temp_input || !temp_output) {
        set_error("Failed to allocate memory for temporary buffers");
        secure_free((void**)&temp_input);
        secure_free((void**)&temp_output);
        return -1;
    }

    memcpy(temp_input, input, input_size * sizeof(float));

    for (size_t i = 0; i < model->num_layers; i++) {
        const Layer* layer = &model->layers[i];
        for (size_t j = 0; j < layer->rows; j++) {
            float sum = 0;
            for (size_t k = 0; k < layer->cols; k++) {
                sum += layer->weights[j * layer->cols + k] * temp_input[k];
            }
            temp_output[j] = (sum > 0) ? sum : 0;  // ReLU activation
        }
        memcpy(temp_input, temp_output, layer->rows * sizeof(float));
    }

    memcpy(output, temp_output, output_size * sizeof(float));

    secure_free((void**)&temp_input);
    secure_free((void**)&temp_output);

    return 0;
}

void free_model(Model* model) {
    if (model) {
        printf("Freeing model at %p\n", (void*)model);
        for (size_t i = 0; i < model->num_layers; i++) {
            printf("Freeing layer %zu weights at %p\n", i, (void*)model->layers[i].weights);
            if (model->layers[i].weights) {
                secure_free((void**)&model->layers[i].weights);
            }
        }
        if (model->public_key) {
            printf("Freeing model public key at %p\n", (void*)model->public_key);
            secure_free((void**)&model->public_key);
        }
        secure_free((void**)&model);
    } else {
        printf("Attempted to free NULL model.\n");
    }
}

int get_model_public_key(const Model* model, const uint8_t** public_key, size_t* public_key_len) {
    if (!model || !public_key || !public_key_len) {
        set_error("Invalid parameters for get_model_public_key");
        return -1;
    }

    *public_key = model->public_key;
    *public_key_len = model->public_key_len;

    return 0;
}
