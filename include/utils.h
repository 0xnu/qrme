#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Securely allocate memory and zero it out
 *
 * @param size The number of bytes to allocate
 * @return A pointer to the allocated memory, or NULL on failure
 */
void* secure_malloc(size_t size);

/**
 * Securely free memory and set pointer to NULL
 *
 * @param ptr Pointer to the memory to free
 */
void secure_free(void** ptr);

/**
 * Convert a float array to a byte array
 *
 * @param float_array The input float array
 * @param float_array_len The length of the float array
 * @param byte_array Pointer to store the output byte array
 * @param byte_array_len Pointer to store the length of the byte array
 * @return 0 on success, -1 on failure
 */
int float_to_byte_array(const float* float_array, size_t float_array_len,
                        uint8_t** byte_array, size_t* byte_array_len);

/**
 * Convert a byte array to a float array
 *
 * @param byte_array The input byte array
 * @param byte_array_len The length of the byte array
 * @param float_array Pointer to store the output float array
 * @param float_array_len Pointer to store the length of the float array
 * @return 0 on success, -1 on failure
 */
int byte_to_float_array(const uint8_t* byte_array, size_t byte_array_len,
                        float** float_array, size_t* float_array_len);

/**
 * Generate a random float array
 *
 * @param len The length of the array to generate
 * @param min The minimum value (inclusive)
 * @param max The maximum value (exclusive)
 * @return A pointer to the generated array, or NULL on failure
 */
float* generate_random_float_array(size_t len, float min, float max);

/**
 * Compute the dot product of two float arrays
 *
 * @param a The first array
 * @param b The second array
 * @param len The length of both arrays
 * @return The dot product
 */
float dot_product(const float* a, const float* b, size_t len);

/**
 * Perform element-wise addition of two float arrays
 *
 * @param a The first array
 * @param b The second array
 * @param result The array to store the result
 * @param len The length of all arrays
 */
void vector_add(const float* a, const float* b, float* result, size_t len);

/**
 * Perform element-wise subtraction of two float arrays
 *
 * @param a The first array
 * @param b The second array
 * @param result The array to store the result
 * @param len The length of all arrays
 */
void vector_subtract(const float* a, const float* b, float* result, size_t len);

/**
 * Compute the L2 norm (Euclidean norm) of a float array
 *
 * @param a The input array
 * @param len The length of the array
 * @return The L2 norm
 */
float l2_norm(const float* a, size_t len);

/**
 * Normalize a float array to have unit L2 norm
 *
 * @param a The input array
 * @param result The array to store the normalized result
 * @param len The length of both arrays
 */
void normalize(const float* a, float* result, size_t len);

/**
 * Compute the softmax of a float array
 *
 * @param a The input array
 * @param result The array to store the softmax result
 * @param len The length of both arrays
 */
void softmax(const float* a, float* result, size_t len);

/**
 * Print a float array
 *
 * @param array The array to print
 * @param len The length of the array
 * @param name The name of the array (for display purposes)
 */
void print_float_array(const float* array, size_t len, const char* name);

/**
 * Save a float array to a file
 *
 * @param filename The name of the file to save to
 * @param array The array to save
 * @param len The length of the array
 * @return 0 on success, -1 on failure
 */
int save_float_array(const char* filename, const float* array, size_t len);

/**
 * Load a float array from a file
 *
 * @param filename The name of the file to load from
 * @param array Pointer to store the loaded array
 * @param len Pointer to store the length of the loaded array
 * @return 0 on success, -1 on failure
 */
int load_float_array(const char* filename, float** array, size_t* len);

/**
 * Initialize the random number generator
 * This function should be called once at the start of the program
 */
void init_random(void);

/**
 * Compare two float values with a small epsilon to account for floating-point imprecision
 *
 * @param a The first float
 * @param b The second float
 * @param epsilon The maximum difference to consider the floats equal
 * @return 1 if the floats are equal within epsilon, 0 otherwise
 */
int float_equal(float a, float b, float epsilon);

/**
 * Clip a float value to a specified range
 *
 * @param value The value to clip
 * @param min The minimum allowed value
 * @param max The maximum allowed value
 * @return The clipped value
 */
float clip(float value, float min, float max);

/**
 * Compute the sigmoid of a float value
 *
 * @param x The input value
 * @return The sigmoid of x
 */
float sigmoid(float x);

/**
 * Apply the sigmoid function element-wise to a float array
 *
 * @param a The input array
 * @param result The array to store the result
 * @param len The length of both arrays
 */
void sigmoid_array(const float* a, float* result, size_t len);

/**
 * Compute the hyperbolic tangent (tanh) of a float value
 *
 * @param x The input value
 * @return The tanh of x
 */
float tanh_float(float x);

/**
 * Apply the tanh function element-wise to a float array
 *
 * @param a The input array
 * @param result The array to store the result
 * @param len The length of both arrays
 */
void tanh_array(const float* a, float* result, size_t len);

/**
 * Compute the ReLU (Rectified Linear Unit) of a float value
 *
 * @param x The input value
 * @return The ReLU of x
 */
float relu(float x);

/**
 * Apply the ReLU function element-wise to a float array
 *
 * @param a The input array
 * @param result The array to store the result
 * @param len The length of both arrays
 */
void relu_array(const float* a, float* result, size_t len);

/**
 * Compute the leaky ReLU of a float value
 *
 * @param x The input value
 * @param alpha The slope for negative values
 * @return The leaky ReLU of x
 */
float leaky_relu(float x, float alpha);

/**
 * Apply the leaky ReLU function element-wise to a float array
 *
 * @param a The input array
 * @param result The array to store the result
 * @param len The length of both arrays
 * @param alpha The slope for negative values
 */
void leaky_relu_array(const float* a, float* result, size_t len, float alpha);

/**
 * Get the last error message from the utils module
 *
 * @return The last error message
 */
const char* get_utils_error(void);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */
