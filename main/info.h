#pragma once

typedef struct {
    int no_empty_sendqueue;
    int no_busy_sendqueue;
    int no_sendqueue_waiting;
    int total_sendqueue;
    int max_send_len;
    int max_receive_len;
    int total_response_time;
    int no_responses;
    int max_response_time;
    int elapsed_ticks;
    int polls;
    int pending;
    int valid;
    int updates;
    int send_queue_full;
    int send_queue_fast_full;
    int uart_max_read_len;
    int uart_buffer_full;
    int uart_fifo_ovf;
    int input_queue_full;
} info_t;

info_t *get_info();
void register_info_endpoints(void);
