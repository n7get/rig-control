<template>
    <b-row>
        <div @click="sendToggle" class="col-4" :class="{disabled: disabled}">
            {{ label }}<span v-if="show !== 'none'"> ({{ value.value }})</span>
        </div>
        <div class="col-7 px-0">
            <b-form-input
                v-model="value.value"
                type="range"
                :min="min"
                :max="max"
                @change="sendChange"
                :disabled="disabled"
            ></b-form-input>
        </div>
    </b-row>
</template>

<script setup>
import { computed, defineProps, ref } from 'vue';
import { useSettingsStore } from '@/stores/settings';
import { send_command } from '@/js/web_socket.js';

const props = defineProps({
    event: {
        type: String,
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
    'toggle': {
        type: String,
        default: null,
    },
});

const disabled = computed(() => {
    if(!props.toggle) {
        return false;
    }
    return !useSettingsStore().settings[props.toggle].value;
});
const value = useSettingsStore().settings[props.event];

const sendChange = () => {
    send_command(props.event, value.value);
};
const sendToggle = () => {
    if(props.toggle) {
        send_command(props.toggle, disabled.value);
    }
};
</script>

<style scoped>
    .disabled {
        color: lightgray;
    }
</style>
