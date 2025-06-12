#pragma once

#include <stdio.h>
#include <stdint.h>
#include "cJSON.h"

#include "rig.h"
#include "linked_list.h"
#include "op_mode.h"

#define MEM_CHaN_NAMESPACE "mem_chan"
#define MAX_MEM_CHAN_NAME_LEN 32

typedef struct {
    uint32_t id;
    char name[MAX_MEM_CHAN_NAME_LEN + 1];
    uint32_t frequency;
    char op_mode[MAX_OP_MODE_NAME_LEN + 1];
    char *commands; // malloced
} mem_chan_t;

void mc_free_mem_chan(mem_chan_t *chan);
void mc_recv_from_ui(cJSON *json_obj);
void log_mem_chan(char *tag, char *prefix, const mem_chan_t *chan);
mem_chan_t *mc_find_by_name(const char *name);
void init_mem_chan();