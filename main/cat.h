#ifndef CAT_H
#define CAT_H

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "rig_commands.h"

#define SEND_QUEUE_SIZE 25
#define SEND_QUEUE_MIN 2

#define SEND_TYPE_COMMAND 0
#define SEND_TYPE_MONITOR 1

#define SEND_PRIORITY_NORMAL 0
#define SEND_PRIORITY_HIGH 1

#define RESPONSE_TIMEOUT_MS 500

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

esp_err_t cat_send(const char *command, int type, int priority);
int cat_send_len();
void cat_flush();
esp_err_t cat_init(void);
void log_command_buf(const char *tag, command_buf_t *command_buf);
void log_result_buf(const char *tag, result_buf_t *result);

#endif // CAT_H
