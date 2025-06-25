#include <string.h>
#include <stdlib.h>
#include "esp_log.h"

#include "command_list.h"
#include "rig_commands.h"

#define TAG "COMMAND_LIST"

// Helper for freeing memory safely
static void safe_free(void *ptr) {
    if (ptr) {
        free(ptr);
    }
}

linked_list_t *command_list_create(char *cmd_str) {
    if (cmd_str == NULL) {
        ESP_LOGE(TAG, "Invalid parameters: cmd_str is NULL");
        return NULL;
    }

    linked_list_t *list = linked_list_create();
    if (list == NULL) {
        ESP_LOGE(TAG, "Failed to create linked list");
        return NULL;
    }

    if (strlen(cmd_str) == 0) {
        return list;
    }

    char *commands = strdup(cmd_str);
    if (commands == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for command string");
        linked_list_destroy(list, safe_free);
        return NULL;
    }

    char *token, *string = commands;
    while ((token = strsep(&string, ";")) != NULL) {
        if (strlen(token) > 0) {
            char *data = malloc(strlen(token) + 2);
            if (data == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for command data");
                linked_list_destroy(list, safe_free);
                safe_free(commands);
                return NULL;
            }
            strcpy(data, token);
            strcat(data, ";");
            linked_list_push(list, data);
        }
    }

    safe_free(commands);

    return list;
}

static linked_list_node_t *find_command_in_list(const linked_list_t *list, const char *command) {
    if (list == NULL || command == NULL) {
        return NULL;
    }
    for(linked_list_node_t *node = linked_list_begin(list); node != NULL; node = linked_list_next(node)) {
        if (strcmp((char *)node->data, command) == 0) {
            return node;
        }
    }
    return NULL;
}

void command_list_merge(linked_list_t *list1, linked_list_t *list2) {
    if (list1 == NULL || list2 == NULL) {
        ESP_LOGE(TAG, "One of the command lists is NULL");
        return;
    }

    char *cmd2 = (char *)linked_list_pop(list2);
    while (cmd2 != NULL) {
        linked_list_node_t *existing_node = find_command_in_list(list1, cmd2);
        if (existing_node == NULL) {
            linked_list_push(list1, cmd2);
        } else {
            safe_free(existing_node->data);
            existing_node->data = cmd2;
        }
        cmd2 = (char *)linked_list_pop(list2);
    }
}

static int node_order(const char *cmd) {
    if (rc_is_freq_command(cmd)) {
        return 1;
    }
    if (rc_is_mode_command(cmd)) {
        return 2;
    }
    if (rc_is_narrow_command(cmd)) {
        return 3;
    }
    if (rc_is_width_command(cmd)) {
        return 4;
    }
    return 10;
}

static int node_order_compare(const void *a, const void *b) {
    int order_a = node_order(a);
    int order_b = node_order(b);
    if (order_a < order_b) {
        return -1; // cmd_a comes before cmd_b
    } else if (order_a > order_b) {
        return 1; // cmd_a comes after cmd_b
    } else {
        return 0; // cmd_a and cmd_b are equal in order
    }
}

void command_list_sort(linked_list_t *list) {
    linked_list_sort(list, node_order_compare);
}
