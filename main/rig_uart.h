#ifndef RIG_UART_H
#define RIG_UART_H

#include <stdio.h>
#include "buffer.h"

// UART configuration
#define UART_NUM UART_NUM_1
#define UART_BAUD_RATE 38400
#define INPUT_QUEUE_SIZE 10

esp_err_t rig_uart_init(void);
int rig_uart_send(char *data, size_t len);
int rig_uart_recv(char *data, size_t len, char terminator);
int rig_uart_recv_len(void);
int rig_uart_recv_byte(char *data);
int rig_uart_peek_buffer(char *data, size_t len);
void rig_uart_clear_buffer(void);
void rig_uart_flush(void);

#endif // RIG_UART_H
