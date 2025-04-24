#include <stdlib.h>
#include "observer.h"

subject_t *new_subject() {
    subject_t *subject = (subject_t *)malloc(sizeof(subject_t));
    if (subject == NULL) {
        return NULL;
    }
    subject->head = NULL;
    subject->tail = NULL;
    return subject;
}

void add_observer(subject_t *subject, observer_callback_t callback, void *context) {
    observer_t *new_observer = (observer_t *)malloc(sizeof(observer_t));
    if (new_observer == NULL) {
        return; // Handle memory allocation failure
    }
    new_observer->callback = callback;
    new_observer->context = context;
    new_observer->prev = NULL;
    new_observer->next = NULL;

    if (subject->tail == NULL) {
        // First observer
        subject->head = new_observer;
        subject->tail = new_observer;
    } else {
        // Append to the end of the list
        subject->tail->next = new_observer;
        new_observer->prev = subject->tail;
        subject->tail = new_observer;
    }
}

void remove_observer(subject_t *subject, observer_callback_t callback, void *context) {
    observer_t *current = subject->head;
    while (current != NULL) {
        if (current->callback == callback && current->context == context) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            } else {
                subject->head = current->next;
            }

            if (current->next != NULL) {
                current->next->prev = current->prev;
            } else {
                subject->tail = current->prev;
            }

            free(current);
            return;
        }
        current = current->next;
    }
}

void subject_notify(subject_t *subject, void *data) {
    observer_t *current = subject->head;
    while (current != NULL) {
        current->callback(current->context, data);
        current = current->next;
    }
}

void free_subject(subject_t *subject) {
    observer_t *current = subject->head;
    while (current != NULL) {
        observer_t *next = current->next;
        free(current);
        current = next;
    }
    free(subject);
}