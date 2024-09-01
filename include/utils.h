#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Securely reallocate memory
 *
 * @param ptr Pointer to the memory block to be reallocated
 * @param size The new size of the memory block
 * @return A pointer to the reallocated memory, or NULL on failure
 */
void* secure_realloc(void* ptr, size_t size);

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
 * Print a float array
 *
 * @param array The array to print
 * @param len The length of the array
 * @param name The name of the array (for display purposes)
 */
void print_float_array(const float* array, size_t len, const char* name);

/**
 * Initialize the random number generator
 * This function should be called once at the start of the program
 */
void init_random(void);

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
