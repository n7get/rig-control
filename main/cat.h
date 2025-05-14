#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "rig_commands.h"

#define SEND_QUEUE_SIZE 25
#define SEND_QUEUE_MIN 5

esp_err_t cat_queue_command(command_t *command, int priority);
int cat_send_len();
void cat_flush();
esp_err_t cat_init(void);
void log_response(const char *tag, response_t *result);
