#ifndef UART_H
#define UART_H

#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>
#include "observer.h"
#include "rig_commands.h"

// UART configuration
#define UART_NUM UART_NUM_1
#define UART_BAUD_RATE 38400
#define BUF_SIZE 1024

#define DATA_QUEUE_SIZE 1024
#define SEND_QUEUE_SIZE 500
#define RESPONSE_TIMEOUT_MS 1000

#define SEND_TYPE_COMMAND 0
#define SEND_TYPE_MONITOR 1

#define SEND_PRIORITY_NORMAL 0
#define SEND_PRIORITY_HIGH 1

typedef struct {
    int type;
    char data[SEND_BUFFER_SIZE];
    size_t len;
} command_buf_t;

typedef enum {
    RECV_RESULT_OK = 0,
    RECV_RESULT_OVERFLOW,
    RECV_RESULT_TIMEOUT,
    RECV_RESULT_ERROR,
    // RECV_RESULT_NOT_READY,
} recv_result_t;

typedef struct {
    recv_result_t result;
    command_buf_t command_buf;
    char data[RECV_BUFFER_SIZE];
    size_t len;
} result_buf_t;

esp_err_t cat_init(void);
void cat_clear(void);
esp_err_t cat_send(const char *command, int type, int priority);
void log_command_buf(const char *tag, command_buf_t *command_buf);
void log_result_buf(const char *tag, result_buf_t *result);

#endif // UART_H
