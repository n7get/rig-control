# pragma once

#include "cJSON.h"

#define GROUP_NAMESPACE "group"
#define MAX_GROUP_NAME_LEN 32

typedef struct {
    int id;
    char name[MAX_GROUP_NAME_LEN + 1];
    char *modes;
    int order;
    char *properties;
} group_t;

void gr_recv_from_ui(cJSON *json_obj);
void group_init();
