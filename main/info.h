#pragma once

#include "cJSON.h"

typedef struct {
    // cat.c
    int cat_empty_queue;
    int cat_queue_busy;
    int cat_max_queue_size;
    int cat_max_send_len;
    int cat_max_receive_len;
    int cat_total_response_time;
    int cat_no_responses;
    int cat_max_response_time;
    int cat_queue_fast_full;
    int cat_queue_full;

    // rig_commands.c
    int rc_elapsed_ticks;
    int rc_polls;
    int rc_pending;
    int rc_valid;
    int rc_updates;

    // rig_controller.c
    int rt_empty_queue;
    int rt_queue_busy;
    int rt_max_queue_size;

    // rig_monitor.c
    int rm_empty_queue;
    int rm_queue_busy;
    int rm_max_queue_size;
    int rm_last_queue_event;

    // uart.c
    int uart_empty_queue;
    int uart_queue_busy;
    int uart_max_queue_size;
    int uart_max_read_len;
    int uart_buffer_full;
    int uart_fifo_ovf;
    int uart_input_queue_full;

    // web_socket.c
    int ws_max_queue_size;
    int ws_empty_queue;
    int ws_queue_busy;
} info_t;

info_t *get_info();
void in_recv_from_ui(cJSON *json_obj);
void init_info(void);
