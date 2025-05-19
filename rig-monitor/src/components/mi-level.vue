<template>
    <b-row>
        <div class="col-4">
            {{ label }}<span v-if="show !== 'none'"> ({{ value.value }})</span>
        </div>
        <div class="col-7">
            <b-form-input
                v-model="value.value"
                type="range"
                :min="min"
                :max="max"
                @change="sendChange"
            ></b-form-input>
        </div>
    </b-row>
</template>

<script setup>
import { computed, ref } from 'vue';
import { useMenuStore } from '@/stores/menu.js';
import { send_command } from '@/js/web_socket.js';

const props = defineProps({
    no: {
        type: Number,
        required: true,
    },
    label: {
        type: String,
        required: true,
    },
    min: {
        type: Number,
        default: 0,
    },
    max: {
        type: Number,
        max: 100,
    },
    show: {
        type: String,
        default: '',
    },
});

const value = useMenuStore().menus[props.no];

const sendChange = () => {
    send_command('menu', { no: props.no, value: value.value });
};
</script>
