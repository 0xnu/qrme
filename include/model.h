#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LAYERS 10

typedef struct {
    float* weights;
    size_t rows;
    size_t cols;
    int is_secure_allocated;
} Layer;

typedef struct Model {
    Layer layers[MAX_LAYERS];
    size_t num_layers;
    uint8_t* public_key;
    size_t public_key_len;
} Model;

/**
 * Securely reallocate memory for model operations
 *
 * @param ptr Pointer to the memory block to be reallocated
 * @param size The new size of the memory block
 * @return A pointer to the reallocated memory, or NULL on failure
 */
void* secure_realloc(void* ptr, size_t size);

/**
 * Create a new empty model
 *
 * @return A pointer to the new Model, or NULL on failure
 */
Model* create_model(void);

/**
 * Add a layer to the model
 *
 * @param model The model to add the layer to
 * @param weights The weights of the layer
 * @param rows The number of rows in the weight matrix
 * @param cols The number of columns in the weight matrix
 * @return 0 on success, -1 on failure
 */
int add_layer(Model* model, const float* weights, size_t rows, size_t cols);

/**
 * Save a model to a file
 *
 * @param model The model to save
 * @param filename The name of the file to save the model to
 * @param public_key The public key to encrypt the model
 * @param public_key_len The length of the public key
 * @return 0 on success, -1 on failure
 */
int save_model(const Model* model, const char* filename, const uint8_t* public_key, size_t public_key_len);

/**
 * Load an encrypted model from a file
 *
 * @param filename The name of the file containing the encrypted model
 * @param secret_key The secret key to decrypt the model
 * @param secret_key_len The length of the secret key
 * @return A pointer to the loaded Model, or NULL on failure
 */
Model* load_model(const char* filename, const uint8_t* secret_key, size_t secret_key_len);

/**
 * Perform inference using the model
 *
 * @param model The model to use for inference
 * @param input The input data
 * @param input_size The size of the input data
 * @param output The output data (must be pre-allocated)
 * @param output_size The size of the output data
 * @return 0 on success, -1 on failure
 */
int inference(const Model* model, const float* input, size_t input_size,
              float* output, size_t output_size);

/**
 * Free the memory used by a model
 *
 * @param model The model to free
 */
void free_model(Model* model);

/**
 * Get the public key of the model
 *
 * @param model The model
 * @param public_key Pointer to store the public key
 * @param public_key_len Pointer to store the length of the public key
 * @return 0 on success, -1 on failure
 */
int get_model_public_key(const Model* model, const uint8_t** public_key, size_t* public_key_len);

/**
 * Get the last error message from the model module
 *
 * @return The last error message
 */
const char* get_model_error(void);

#ifdef __cplusplus
}
#endif

#endif /* MODEL_H */
