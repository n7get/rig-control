#ifndef UART_H
#define UART_H

#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>
#include "observer.h"

// UART configuration
#define UART_NUM UART_NUM_1
#define UART_BAUD_RATE 38400
#define BUF_SIZE 1024

#define DATA_QUEUE_SIZE 1024
#define RESPONSE_TIMEOUT_MS 1000
#define SEND_QUEUE_SIZE 100
#define RECV_QUEUE_SIZE 10
#define RESPONSE_TIMEOUT_MS 1000
#define SEND_BUFFER_SIZE 64
#define RECV_BUFFER_SIZE 64

#define SEND_TYPE_EXTERNAL 0    // Put command at front of queue
#define SEND_TYPE_MONITOR 1     // Put command at back of queue
typedef struct {
    int type;
    char data[SEND_BUFFER_SIZE];
    size_t len;
} send_data_t;

typedef struct {
    int type;
    char data[RECV_BUFFER_SIZE];
    size_t len;
    esp_err_t error;
} recv_result_t;

// Function prototypes
esp_err_t uart_init(void);
void uart_add_recv_observer(observer_callback_t callback, void *context);
esp_err_t uart_send(const char *command, int type);

#endif // UART_H
