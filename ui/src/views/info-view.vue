<template>    
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>System Info</b></div>
        <div @click="refresh"><iconsRefresh /></div>
    </div>

    <div v-if="error_message" class="alert alert-danger mt-2 d-flex justify-content-between">
        <div class="me-2"><iconsWarning /></div>
        <div>{{ error_message }}</div>
    </div>

    <div class="mt-3">CAT Queue Statistics:</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Queue Busy (%)</div>
        <div>{{ format(info.cat_queue_busy / (info.cat_empty_queue + info.cat_queue_busy)) * 100 }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Max Queue Size</div>
        <div>{{ format(info.cat_max_queue_size) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Queue Full Count</div>
        <div>{{ info.cat_queue_full }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Queue Fast Full Count</div>
        <div>{{ info.cat_queue_fast_full }}</div>
    </div>

    <div class="mt-3">CAT Response Statistics:</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Response Time (ms)</div>
        <div>{{ format(info.cat_avg_response_time) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Max Response Time (ms)</div>
        <div>{{ info.cat_max_response_time }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Max Send Length</div>
        <div>{{ info.cat_max_send_len }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Max Receive Length</div>
        <div>{{ info.cat_max_receive_len }}</div>
    </div>

    <div class="mt-3">UART Statistics:</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Queue Busy (%)</div>
        <div>{{ format(info.uart_queue_busy / (info.uart_empty_queue + info.uart_queue_busy)) * 100 }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Max Queue Size</div>
        <div>{{ info.uart_max_queue_size }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Max Read Length</div>
        <div>{{ info.uart_max_read_len }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Buffer Full Count</div>
        <div>{{ info.uart_buffer_full }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>FIFO Overflow Count</div>
        <div>{{ info.uart_fifo_ovf }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Input Queue Full Count</div>
        <div>{{ info.uart_input_queue_full }}</div>
    </div>

    <div class="mt-3">Rig Commands Statistics:</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Elapsed Ticks</div>
        <div>{{ format(info.rc_avg_elapsed_ticks) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Pending</div>
        <div>{{ format(info.rc_avg_pending) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Queue Size</div>
        <div>{{ format(info.rc_avg_queue_size) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Updates</div>
        <div>{{ format(info.rc_avg_updates) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Valid</div>
        <div>{{ format(info.rc_avg_valid) }}</div>
    </div>

    <div class="mt-3">Rig Controller</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Queue Busy (%)</div>
        <div v-if="info.rt_empty_queue !== 0 && info.rt_queue_busy !== 0">{{ format(info.rt_queue_busy / (info.rt_empty_queue + info.rt_queue_busy)) * 100 }}</div>
        <div v-else>0</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Max Queue Size</div>
        <div>{{ info.rt_max_queue_size }}</div>
    </div>

    <div class="mt-3">Rig Monitor:</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Queue Busy (%)</div>
        <div>{{ format(info.rm_queue_busy / (info.rm_queue_busy + info.rm_empty_queue)) * 100 }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Max Queue Size</div>
        <div>{{ info.rm_max_queue_size }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Last Queue Event</div>
        <div>{{ info.rm_last_queue_event }}</div>
    </div>

    <div class="mt-3">WebSocket Statistics:</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Average Queue Busy (%)</div>
        <div>{{ format(info.ws_queue_busy / (info.ws_queue_busy + info.ws_empty_queue)) * 100 }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Max Queue Size</div>
        <div>{{ info.ws_max_queue_size }}</div>
    </div>
    
    <div class="mt-3">Memory Statistics:</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Free Heap (bytes)</div>
        <div>{{ formatBytes(info.free_heap) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Minimum Free Heap (bytes)</div>
        <div>{{ formatBytes(info.min_free_heap) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Free Stack (bytes)</div>
        <div>{{ formatBytes(info.free_stack) }}</div>
    </div>

    <div class="mt-3">NVS (Non-Volatile Storage) Statistics:</div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Total Bytes</div>
        <div>{{ formatBytes(info.nvs_total_bytes) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Used Bytes</div>
        <div>{{ formatBytes(info.nvs_used_bytes) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Free Bytes</div>
        <div>{{ formatBytes(info.nvs_free_bytes) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Available Bytes</div>
        <div>{{ formatBytes(info.nvs_available_bytes) }}</div>
    </div>
    <div class="ms-3 d-flex justify-content-between">
        <div>Namespace Bytes</div>
        <div>{{ formatBytes(info.nvs_namespacbytes) }}</div>
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

function formatBytes(bytes) {
    if (typeof bytes !== 'number' || bytes === null) {
        return 'N/A';
    }
    if (bytes === 0) return '0 Bytes';
    
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

function back() {
    router.back();
}

</script>
