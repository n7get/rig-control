#pragma once

#include "linked_list.h"
#include <stdbool.h>
#include "esp_err.h"

linked_list_t *command_list_create(char *cmd_str);
void command_list_merge(linked_list_t *list1, linked_list_t *list2);
void command_list_sort(linked_list_t *list);
