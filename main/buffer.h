#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdint.h>

// Callback function type to fill the buffer with more data.
// Should return the number of bytes read, 0 if no more data, or -1 on error.
typedef int (*buffer_fill_callback_t)(void *context, uint8_t *buf, size_t max_len);

// Buffer structure
typedef struct {
    uint8_t *data;                  // Pointer to the data storage
    size_t size;                    // Total size of the data storage
    size_t count;                   // Number of bytes currently in the buffer
    size_t read_pos;                // Current read position
    buffer_fill_callback_t fill_cb; // Callback to get more data
    void *context;                  // User context for the callback
} buffer_t;

/**
 * @brief Initializes the buffer structure.
 *
 * Allocates memory for the buffer structure and its internal data buffer.
 *
 * @param buf_size Size of the internal data buffer to allocate.
 * @param callback Function pointer to the callback used to fill the buffer.
 * @param context User context pointer to be passed to the callback.
 * @return Pointer to the initialized buffer_t structure, or NULL on error (e.g., memory allocation failed).
 */
buffer_t *buffer_create(size_t buf_size, buffer_fill_callback_t callback, void *context);

/**
 * @brief Gets the next byte from the buffer.
 *
 * If the buffer is empty, it attempts to refill it using the callback function.
 *
 * @param buffer Pointer to the initialized buffer_t structure.
 * @param byte Pointer to store the retrieved byte.
 * @return 1 if a byte was successfully retrieved, 0 if end of data, -1 on error.
 */
int buffer_get_byte(buffer_t *buffer, uint8_t *byte);

/**
 * @brief Frees the memory allocated for the buffer structure and its data.
 *
 * @param buffer Pointer to the buffer_t structure to free.
 */
void buffer_free(buffer_t *buffer);

#endif // BUFFER_H
