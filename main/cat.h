#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "rig_commands.h"

#define SEND_QUEUE_SIZE 25
#define SEND_QUEUE_MIN 5

esp_err_t cat_send(const char *command, int type, int priority);
int cat_send_len();
void cat_flush();
esp_err_t cat_init(void);
void log_command_buf(const char *tag, command_buf_t *command_buf);
void log_result_buf(const char *tag, result_buf_t *result);
