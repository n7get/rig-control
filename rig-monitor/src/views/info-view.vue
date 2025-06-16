<template>    
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Config</b></div>
        <div @click="refresh"><iconsRefresh /></div>
    </div>

    <div v-if="error_message" class="alert alert-danger mt-2 d-flex justify-content-between">
        <div class="me-2"><iconsWarning /></div>
        <div>{{ error_message }}</div>
    </div>

    <div class="mt-3">UART Queue info:</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>busy_sendqueue_percent</div>
        <div>{{ format(info.busy_sendqueue_percent) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>avg_sendqueue_len</div>
        <div>{{ format(info.avg_sendqueue_len) }}</div>
    </div>

    <div class="mt-3">UART info:</div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>max_send_len</div>
	    <div>{{ info.max_send_len }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>max_receive_len</div>
	    <div>{{ info.max_receive_len }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>avg_response_time</div>
	    <div>{{ format(info.avg_response_time) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>max_response_time</div>
	    <div>{{ info.max_response_time }}</div>
    </div>
    
    <div class="mt-3">Event queue info:</div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>avg_elapsed_ticks</div>
	    <div>{{ info.avg_elapsed_ticks }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>avg_pending</div>
	    <div>{{ format(info.avg_pending) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>avg_valid</div>
	    <div>{{ format(info.avg_valid) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>avg_updates</div>
	    <div>{{ format(info.avg_updates) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>avg_rm_queue_size</div>
	    <div>{{ info.avg_rm_queue_size }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>last_rm_queue_event</div>
	    <div>{{ info.last_rm_queue_event }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>cat_queue_full</div>
	    <div>{{ info.cat_queue_full }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>cat_queue_fast_full</div>
	    <div>{{ info.cat_queue_fast_full }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>uart_max_read_len</div>
	    <div>{{ info.uart_max_read_len }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>uart_buffer_full</div>
	    <div>{{ info.uart_buffer_full }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>uart_fifo_ovf</div>
	    <div>{{ info.uart_fifo_ovf }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>input_queue_full</div>
	    <div>{{ info.input_queue_full }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>avg_ws_queue_size</div>
	    <div>{{ info.avg_ws_queue_size }}</div>
    </div>
    
    <div class="mt-3">Memory info:</div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>free_heap</div>
	    <div>{{ info.free_heap }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>min_free_heap</div>
	    <div>{{ info.min_free_heap }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>free_stack</div>
	    <div>{{ info.free_stack }}</div>
    </div>
    
    <div class="mt-3">NVS info:</div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>nvs_used_bytes</div>
	    <div>{{ info.nvs_used_bytes }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>nvs_free_bytes</div>
	    <div>{{ info.nvs_free_bytes }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>nvs_available_bytes</div>
	    <div>{{ info.nvs_available_bytes }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>nvs_total_bytes</div>
	    <div>{{ info.nvs_total_bytes }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
	    <div>nvs_namespacbytes</div>
	    <div>{{ info.nvs_namespacbytes }}</div>
    </div>
</template>

<script setup>
import { computed, ref, onBeforeMount } from 'vue';
import { useRouter } from 'vue-router';
import { useGlobalStore } from '@/stores/global';
import iconsRefresh from '@/components/icons/refresh.vue';
import iconsWarning from '@/components/icons/warning.vue';
import { send_message_with_response } from '@/js/web_socket.js';

const router = useRouter();

const error_message = ref('');
const info = ref({});

onBeforeMount(() => {
    refresh();
});

function refresh() {
    error_message.value = '';
    send_message_with_response({ topic: 'info', event: 'get' })
        .then(response => {
            info.value = response;
        })
        .catch(error => {
            error_message.value = 'Error getting info: ' + error;
        });
}

function format(value) {
    if (typeof value !== 'number') {
        return value;
    }
    if (value !== Math.floor(value)) {
        return value.toFixed(2);
    }
    return value;
}

function back() {
    router.back();
}

</script>
