#include "buffer.h"
#include <string.h> // For NULL
#include <stdlib.h> // For malloc and free

/**
 * @brief Initializes the buffer structure.
 */
buffer_t *buffer_create(size_t buf_size, buffer_fill_callback_t callback, void *context) {
    if (buf_size == 0 || callback == NULL) {
        return NULL; // Invalid arguments
    }

    // Allocate memory for the buffer structure
    buffer_t *buffer = (buffer_t *)malloc(sizeof(buffer_t));
    if (buffer == NULL) {
        return NULL; // Memory allocation failed
    }

    // Allocate memory for the data buffer
    buffer->data = (uint8_t *)malloc(buf_size);
    if (buffer->data == NULL) {
        free(buffer); // Free the allocated buffer structure
        return NULL; // Memory allocation failed
    }

    buffer->size = buf_size;
    buffer->count = 0;
    buffer->read_pos = 0;
    buffer->fill_cb = callback;
    buffer->context = context;

    return buffer;
}

/**
 * @brief Gets the next byte from the buffer.
 */
int buffer_get_byte(buffer_t *buffer, uint8_t *byte) {
    if (!buffer || !byte) {
        return -1; // Invalid arguments
    }

    // If the buffer is empty, try to refill it
    if (buffer->read_pos >= buffer->count) {
        // Pass the context to the callback
        int bytes_read = buffer->fill_cb(buffer->context, buffer->data, buffer->size);
        if (bytes_read < 0) {
            return -1; // Error during fill
        }
        if (bytes_read == 0) {
            return 0; // End of data
        }
        buffer->count = bytes_read;
        buffer->read_pos = 0;
    }

    // Get the next byte
    *byte = buffer->data[buffer->read_pos++];

    return 1; // Byte successfully retrieved
}

/**
 * @brief Frees the memory allocated for the buffer structure and its data.
 */
void buffer_free(buffer_t *buffer) {
    if (buffer != NULL) {
        if (buffer->data != NULL) {
            free(buffer->data);
        }
        free(buffer);
    }
}