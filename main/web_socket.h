#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"

#define WS_SERVER_PORT 8080
#define WS_MAX_CLIENTS 4
#define WS_RECV_BUFFER_SIZE 2048

// WebSocket frame opcodes
#define WS_OPCODE_CONTINUATION 0x0
#define WS_OPCODE_TEXT         0x1
#define WS_OPCODE_BINARY       0x2
#define WS_OPCODE_CLOSE        0x8
#define WS_OPCODE_PING         0x9
#define WS_OPCODE_PONG         0xA


#include "freertos/queue.h"

// Start the WebSocket server. Provide a queue for outgoing messages.
void ws_server_start();
void ws_server_stop(void);
void ws_send(const char *data);
