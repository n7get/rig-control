#pragma once

#define RESPONSE_TIMEOUT_MS 500

#define SEND_BUFFER_SIZE 64
#define RECV_BUFFER_SIZE 64

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
