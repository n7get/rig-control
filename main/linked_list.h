#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>
#include "esp_err.h"

// Node structure for the linked list
typedef struct linked_list_node {
    void *data;
    struct linked_list_node *next;
} linked_list_node_t;

// Linked list structure
typedef struct linked_list {
    linked_list_node_t *head;
    linked_list_node_t *tail;
    size_t size;
} linked_list_t;

// Function pointer type for freeing node data
typedef void (*linked_list_free_fn)(void *data);

// Create a new linked list
linked_list_t *linked_list_create(void);

// Clear the linked list without destroying it
void linked_list_clear(linked_list_t *list, linked_list_free_fn free_fn);

// Destroy the linked list and free all nodes
void linked_list_destroy(linked_list_t *list, linked_list_free_fn free_fn);

// Pop the first item from the list
void *linked_list_pop(linked_list_t *list);

// Shift an item to the front of the list
void linked_list_shift(linked_list_t *list, void *data);

// Add an item to the end of the list
void linked_list_push(linked_list_t *list, void *data);

// Remove the first node with matching data pointer (shallow compare)
esp_err_t linked_list_remove(linked_list_t *list, void *data);

// Get the size of the list
size_t linked_list_size(const linked_list_t *list);

// Iterate over the list
linked_list_node_t *linked_list_begin(const linked_list_t *list);
linked_list_node_t *linked_list_next(const linked_list_node_t *node);

#endif // LINKED_LIST_H
