#pragma once

#include <stdio.h>
#include <stdint.h>
#include "cJSON.h"
#include "rig.h"

#define OP_MODE_NAMESPACE "op_mode"
#define MAX_OP_MODES 32
#define MAX_OP_MODE_NAME_LEN 14
#define MAX_COMMANDS_BUF_SIZE 512
#define MAX_FREQ_RANGES 16
#define MAX_ORDER 255

typedef struct {
    uint32_t start;
    uint32_t end;
} freq_range_t;

typedef struct {
    uint32_t id;
    char name[MAX_OP_MODE_NAME_LEN + 1];
    freq_range_t freq_ranges[MAX_FREQ_RANGES];
    uint8_t order;
    char commands[MAX_COMMANDS_BUF_SIZE + 1];
} op_mode_t;

void om_recv_from_ui(cJSON *json_obj);
void log_op_mode(char *tag, char *prefix, const op_mode_t *mode);
void init_op_mode();
