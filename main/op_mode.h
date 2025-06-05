#pragma once

#include <stdio.h>
#include <stdint.h>
#include "cJSON.h"

#include "rig.h"
#include "linked_list.h"

#define OP_MODE_NAMESPACE "op_mode"
#define MAX_OP_MODE_NAME_LEN 14
#define MAX_ORDER 255

typedef struct {
    uint32_t start;
    uint32_t end;
} freq_range_t;

typedef struct {
    uint32_t id;
    char name[MAX_OP_MODE_NAME_LEN + 1];
    linked_list_t *freq_ranges;
    uint8_t order;
    char *commands; // malloced
} op_mode_t;

void om_recv_from_ui(cJSON *json_obj);
void log_op_mode(char *tag, char *prefix, const op_mode_t *mode);
void init_op_mode();
esp_err_t om_find_by_frequency(uint32_t frequency, char **name, char **commands);
esp_err_t om_find_by_name(const char *name, char **commands);