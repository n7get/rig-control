#include "esp_err.h"
#include "linked_list.h"
#include <stdlib.h>

// Create a new linked list
linked_list_t *linked_list_create(void) {
    linked_list_t *list = (linked_list_t *)malloc(sizeof(linked_list_t));
    if (list) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
    return list;
}

void linked_list_clear(linked_list_t *list, linked_list_free_fn free_fn) {
    if (!list) return;
    linked_list_node_t *node = list->head;
    while (node) {
        linked_list_node_t *next = node->next;
        if (free_fn) free_fn(node->data);
        free(node);
        node = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

// Destroy the linked list and free all nodes
void linked_list_destroy(linked_list_t *list, linked_list_free_fn free_fn) {
    if (!list) return;
    linked_list_clear(list, free_fn);
    free(list);
}

void *linked_list_pop(linked_list_t *list) {
    if (!list || !list->head) return NULL;
    linked_list_node_t *node = list->head;
    void *data = node->data;
    list->head = node->next;
    if (list->head == NULL) {
        list->tail = NULL;
    }
    free(node);
    list->size--;
    return data;
}

void linked_list_shift(linked_list_t *list, void *data) {
    if (!list) return;
    linked_list_node_t *node = (linked_list_node_t *)malloc(sizeof(linked_list_node_t));
    if (!node) return;
    node->data = data;
    node->next = list->head;
    list->head = node;
    if (list->tail == NULL) {
        list->tail = node;
    }
    list->size++;
}

// Add an item to the end of the list
void linked_list_push(linked_list_t *list, void *data) {
    if (!list) return;
    linked_list_node_t *node = (linked_list_node_t *)malloc(sizeof(linked_list_node_t));
    if (!node) return;
    node->data = data;
    node->next = NULL;
    if (!list->head) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

// Remove the first node with matching data pointer (shallow compare)
esp_err_t linked_list_remove(linked_list_t *list, void *data) {
    if (!list || !list->head) return ESP_FAIL;
    linked_list_node_t *prev = NULL, *node = list->head;
    while (node) {
        if (node->data == data) {
            if (prev) prev->next = node->next;
            else list->head = node->next;
            if (node == list->tail) list->tail = prev;
            free(node);
            list->size--;
            return ESP_OK;
        }
        prev = node;
        node = node->next;
    }
    return ESP_FAIL;
}

// Get the size of the list
size_t linked_list_size(const linked_list_t *list) {
    return list ? list->size : 0;
}

// Iterate over the list
linked_list_node_t *linked_list_begin(const linked_list_t *list) {
    return list ? list->head : NULL;
}

linked_list_node_t *linked_list_next(const linked_list_node_t *node) {
    return node ? node->next : NULL;
}
