#pragma once

#include "cJSON.h"
#include "mem_chan.h"

void rt_recv_from_ui(cJSON *json_obj);
void rt_find_op_node_by_freq(void);
void init_rig_controller(void);
