#ifndef OBSERVER_H
#define OBSERVER_H

#include <stddef.h>

typedef void (*observer_callback_t)(void *context, void *data);

typedef struct observer_t {
    observer_callback_t callback;
    void *context;
    struct observer_t *prev;
    struct observer_t *next;
} observer_t;

typedef struct {
    observer_t *head;
    observer_t *tail;
} subject_t;

subject_t *new_subject();
void add_observer(subject_t *subject, observer_callback_t callback, void *context);
void remove_observer(subject_t *subject, observer_callback_t callback);
void subject_notify(subject_t *subject, void *data);
void free_subject(subject_t *subject);

#endif // OBSERVER_H

