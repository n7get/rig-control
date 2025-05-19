#pragma once

#define RESPONSE_TIMEOUT_MS 500

#define SEND_BUFFER_SIZE 64
#define RECV_BUFFER_SIZE 64

#define SEND_PRIORITY_NORMAL 0
#define SEND_PRIORITY_HIGH 1

typedef enum {
    SEND_TYPE_COMMAND = 0,
    SEND_TYPE_POLL,
} send_type_t;

typedef enum {
    RECV_RESULT_OK = 0,
    RECV_RESULT_OVERFLOW,
    RECV_RESULT_TIMEOUT,
    RECV_RESULT_ERROR,
    // RECV_RESULT_NOT_READY,
} recv_result_t;

typedef struct {
    send_type_t type;

    char command[SEND_BUFFER_SIZE];
    size_t command_len;

    char read[SEND_BUFFER_SIZE];
    size_t read_len;
} command_t;

typedef struct {
    send_type_t type;

    char command[SEND_BUFFER_SIZE];
    size_t command_len;

    char read[SEND_BUFFER_SIZE];
    size_t read_len;

    char response[RECV_BUFFER_SIZE];
    size_t response_len;

    recv_result_t result;
} response_t;
