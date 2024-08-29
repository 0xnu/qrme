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

void secure_free(void** ptr) {
    if (ptr != NULL && *ptr != NULL) {
        // Check if the pointer is aligned correctly
        if ((uintptr_t)*ptr % sizeof(void*) != 0) {
            printf("secure_free: Warning - misaligned pointer %p\n", (void*)*ptr);
            return;
        }

        // Check if we can safely access the memory
        if (((char*)*ptr - sizeof(size_t)) < (char*)*ptr) {
            printf("secure_free: Warning - cannot access memory before %p\n", (void*)*ptr);
            return;
        }

        secure_alloc_t* alloc = (secure_alloc_t*)((char*)*ptr - offsetof(secure_alloc_t, data));

        // Check if the size field looks reasonable
        if (alloc->size == 0 || alloc->size > 1000000000) { // 1GB as an arbitrary large size
            printf("secure_free: Warning - suspicious size %zu for pointer %p\n", alloc->size, (void*)*ptr);
            return;
        }

        printf("secure_free: Freeing %zu bytes at %p (original pointer %p)\n", alloc->size, (void*)alloc, (void*)*ptr);
        memset(alloc->data, 0, alloc->size);
        free(alloc);
        *ptr = NULL;
    } else {
        printf("secure_free: Nothing to free (ptr is NULL or *ptr is NULL)\n");
    }
}

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

float dot_product(const float* a, const float* b, size_t len) {
    float result = 0.0f;
    for (size_t i = 0; i < len; i++) {
        result += a[i] * b[i];
    }
    return result;
}

void vector_add(const float* a, const float* b, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = a[i] + b[i];
    }
}

void vector_subtract(const float* a, const float* b, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = a[i] - b[i];
    }
}

float l2_norm(const float* a, size_t len) {
    float sum = 0.0f;
    for (size_t i = 0; i < len; i++) {
        sum += a[i] * a[i];
    }
    return sqrt(sum);
}

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

void init_random() {
    srand(time(NULL));
}

int float_equal(float a, float b, float epsilon) {
    return fabs(a - b) < epsilon;
}

float clip(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
}

void sigmoid_array(const float* a, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = sigmoid(a[i]);
    }
}

float tanh_float(float x) {
    return tanh(x);
}

void tanh_array(const float* a, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = tanh_float(a[i]);
    }
}

float relu(float x) {
    return x > 0 ? x : 0;
}

void relu_array(const float* a, float* result, size_t len) {
    for (size_t i = 0; i < len; i++) {
        result[i] = relu(a[i]);
    }
}

float leaky_relu(float x, float alpha) {
    return x > 0 ? x : alpha * x;
}

void leaky_relu_array(const float* a, float* result, size_t len, float alpha) {
    for (size_t i = 0; i < len; i++) {
        result[i] = leaky_relu(a[i], alpha);
    }
}
