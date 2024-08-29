#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../include/utils.h"

#define MAX_ERROR_LENGTH 256

// Global error state
static char error_message[MAX_ERROR_LENGTH] = {0};

// Function to set error message
void set_utils_error(const char* message) {
    strncpy(error_message, message, MAX_ERROR_LENGTH - 1);
    error_message[MAX_ERROR_LENGTH - 1] = '\0';
}

// Function to get error message
const char* get_utils_error() {
    return error_message;
}

typedef struct {
    size_t size;
    char data[];
} secure_alloc_t;

/**
 * Securely allocate memory and zero it out
 *
 * @param size The number of bytes to allocate
 * @return A pointer to the allocated memory, or NULL on failure
 */
void* secure_malloc(size_t size) {
     secure_alloc_t* alloc = malloc(sizeof(secure_alloc_t) + size);
     if (alloc) {
         alloc->size = size;
         memset(alloc->data, 0, size);
         printf("secure_malloc: Allocated %zu bytes at %p (returned %p)\n", size, (void*)alloc, (void*)alloc->data);
         return alloc->data;
     }
     set_utils_error("Failed to allocate memory");
     return NULL;
}

/**
 * Securely free memory and set pointer to NULL
 *
 * @param ptr Pointer to the memory to free
 */
void secure_free(void** ptr) {
     if (ptr != NULL && *ptr != NULL) {
         secure_alloc_t* alloc = (secure_alloc_t*)((char*)*ptr - offsetof(secure_alloc_t, data));
         printf("secure_free: Freeing %zu bytes at %p (original pointer %p)\n", alloc->size, (void*)alloc, (void*)*ptr);
         memset(alloc->data, 0, alloc->size);
         free(alloc);
         *ptr = NULL;
     } else {
         printf("secure_free: Nothing to free (ptr is NULL or *ptr is NULL)\n");
     }
}

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
                        uint8_t** byte_array, size_t* byte_array_len) {
    *byte_array_len = float_array_len * sizeof(float);
    *byte_array = secure_malloc(*byte_array_len);
    if (!*byte_array) {
        set_utils_error("Failed to allocate memory for byte array");
        return -1;
    }
    memcpy(*byte_array, float_array, *byte_array_len);
    return 0;
}

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
                        float** float_array, size_t* float_array_len) {
    if (byte_array_len % sizeof(float) != 0) {
        set_utils_error("Byte array length is not a multiple of sizeof(float)");
        return -1;
    }
    *float_array_len = byte_array_len / sizeof(float);
    *float_array = secure_malloc(byte_array_len);
    if (!*float_array) {
        set_utils_error("Failed to allocate memory for float array");
        return -1;
    }
    memcpy(*float_array, byte_array, byte_array_len);
    return 0;
}

/**
 * Generate a random float array
 *
 * @param len The length of the array to generate
 * @param min The minimum value (inclusive)
 * @param max The maximum value (exclusive)
 * @return A pointer to the generated array, or NULL on failure
 */
float* generate_random_float_array(size_t len, float min, float max) {
     float* array = (float*)secure_malloc(len * sizeof(float));
     if (!array) {
         set_utils_error("Failed to allocate memory for random float array");
         return NULL;
     }

     for (size_t i = 0; i < len; i++) {
         array[i] = min + (max - min) * ((float)rand() / RAND_MAX);
     }

     return array;
}

/**
 * Compute the dot product of two float arrays
 *
 * @param a The first array
 * @param b The second array
 * @param len The length of both arrays
 * @return The dot product
 */
float dot_product(const float* a, const float* b, size_t len) {
    float result = 0.0f;
    for (size_t i = 0; i < len; i++) {
        result += a[i] * b[i];
    }
    return result;
}

/**
 * Perform element-wise addition of two float arrays
 *
 * @param a The first array
 * @param b The second array
 * @param result The array to store the result
 * @param len The length of all arrays
 */
void vector_add(const float* a, const float* b, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = a[i] + b[i];
    }
}

/**
 * Perform element-wise subtraction of two float arrays
 *
 * @param a The first array
 * @param b The second array
 * @param result The array to store the result
 * @param len The length of all arrays
 */
void vector_subtract(const float* a, const float* b, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = a[i] - b[i];
    }
}

/**
 * Compute the L2 norm (Euclidean norm) of a float array
 *
 * @param a The input array
 * @param len The length of the array
 * @return The L2 norm
 */
float l2_norm(const float* a, size_t len) {
    float sum = 0.0f;
    for (size_t i = 0; i < len; i++) {
        sum += a[i] * a[i];
    }
    return sqrt(sum);
}

/**
 * Normalize a float array to have unit L2 norm
 *
 * @param a The input array
 * @param result The array to store the normalized result
 * @param len The length of both arrays
 */
void normalize(const float* a, float* result, size_t len) {
    float norm = l2_norm(a, len);
    if (norm == 0) {
        set_utils_error("Cannot normalize zero vector");
        return;
    }
    for (size_t i = 0; i < len; i++) {
        result[i] = a[i] / norm;
    }
}

/**
 * Compute the softmax of a float array
 *
 * @param a The input array
 * @param result The array to store the softmax result
 * @param len The length of both arrays
 */
void softmax(const float* a, float* result, size_t len) {
    float max = a[0];
    for (size_t i = 1; i < len; i++) {
        if (a[i] > max) {
            max = a[i];
        }
    }

    float sum = 0.0f;
    for (size_t i = 0; i < len; i++) {
        result[i] = exp(a[i] - max);
        sum += result[i];
    }

    for (size_t i = 0; i < len; i++) {
        result[i] /= sum;
    }
}

/**
 * Print a float array
 *
 * @param array The array to print
 * @param len The length of the array
 * @param name The name of the array (for display purposes)
 */
void print_float_array(const float* array, size_t len, const char* name) {
    printf("%s: [", name);
    for (size_t i = 0; i < len; i++) {
        printf("%f", array[i]);
        if (i < len - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

/**
 * Save a float array to a file
 *
 * @param filename The name of the file to save to
 * @param array The array to save
 * @param len The length of the array
 * @return 0 on success, -1 on failure
 */
int save_float_array(const char* filename, const float* array, size_t len) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        set_utils_error("Failed to open file for writing");
        return -1;
    }

    size_t written = fwrite(array, sizeof(float), len, file);
    if (written != len) {
        set_utils_error("Failed to write entire array to file");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

/**
 * Load a float array from a file
 *
 * @param filename The name of the file to load from
 * @param array Pointer to store the loaded array
 * @param len Pointer to store the length of the loaded array
 * @return 0 on success, -1 on failure
 */
int load_float_array(const char* filename, float** array, size_t* len) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        set_utils_error("Failed to open file for reading");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size % sizeof(float) != 0) {
        set_utils_error("File size is not a multiple of sizeof(float)");
        fclose(file);
        return -1;
    }

    *len = file_size / sizeof(float);
    *array = secure_malloc(file_size);
    if (!*array) {
        set_utils_error("Failed to allocate memory for loaded array");
        fclose(file);
        return -1;
    }

    size_t read = fread(*array, sizeof(float), *len, file);
    if (read != *len) {
        set_utils_error("Failed to read entire array from file");
        secure_free((void**)array);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

/**
 * Initialize the random number generator
 * This function should be called once at the start of the program
 */
void init_random() {
    srand(time(NULL));
}

/**
 * Compare two float values with a small epsilon to account for floating-point imprecision
 *
 * @param a The first float
 * @param b The second float
 * @param epsilon The maximum difference to consider the floats equal
 * @return 1 if the floats are equal within epsilon, 0 otherwise
 */
int float_equal(float a, float b, float epsilon) {
    return fabs(a - b) < epsilon;
}

/**
 * Clip a float value to a specified range
 *
 * @param value The value to clip
 * @param min The minimum allowed value
 * @param max The maximum allowed value
 * @return The clipped value
 */
float clip(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/**
 * Compute the sigmoid of a float value
 *
 * @param x The input value
 * @return The sigmoid of x
 */
float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
}

/**
 * Apply the sigmoid function element-wise to a float array
 *
 * @param a The input array
 * @param result The array to store the result
 * @param len The length of both arrays
 */
void sigmoid_array(const float* a, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = sigmoid(a[i]);
    }
}

/**
 * Compute the hyperbolic tangent (tanh) of a float value
 *
 * @param x The input value
 * @return The tanh of x
 */
float tanh_float(float x) {
    return tanh(x);
}

/**
 * Apply the tanh function element-wise to a float array
 *
 * @param a The input array
 * @param result The array to store the result
 * @param len The length of both arrays
 */
void tanh_array(const float* a, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = tanh_float(a[i]);
    }
}

/**
 * Compute the ReLU (Rectified Linear Unit) of a float value
 *
 * @param x The input value
 * @return The ReLU of x
 */
float relu(float x) {
    return x > 0 ? x : 0;
}

/**
 * Apply the ReLU function element-wise to a float array
 *
 * @param a The input array
 * @param result The array to store the result
 * @param len The length of both arrays
 */
void relu_array(const float* a, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = relu(a[i]);
    }
}

/**
 * Compute the leaky ReLU of a float value
 *
 * @param x The input value
 * @param alpha The slope for negative values
 * @return The leaky ReLU of x
 */
float leaky_relu(float x, float alpha) {
    return x > 0 ? x : alpha * x;
}

/**
 * Apply the leaky ReLU function element-wise to a float array
 *
 * @param a The input array
 * @param result The array to store the result
 * @param len The length of both arrays
 * @param alpha The slope for negative values
 */
void leaky_relu_array(const float* a, float* result, size_t len, float alpha) {
    for (size_t i = 0; i < len; i++) {
        result[i] = leaky_relu(a[i], alpha);
    }
}
