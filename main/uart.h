#ifndef UART_H
#define UART_H

#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

// UART configuration
#define UART_NUM UART_NUM_1
#define UART_BAUD_RATE 38400
#define BUF_SIZE 1024

// Function prototypes
esp_err_t uart_init(void);
esp_err_t uart_send(const char *command, size_t command_size);
esp_err_t uart_recv(char *response, size_t response_size);

#endif // UART_H
