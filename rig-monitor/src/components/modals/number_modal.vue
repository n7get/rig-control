<template>
    <b-modal
        focus="form-input"
        @ok="close_modal"
        @esc="close_modal"
        v-model="open_modal"
        :title="title"
        ok-only
        ok-title="Close"
    >
        <b-form-input
            id="form-input"
            type="range"
            @change="set(input_value)"
            v-model="input_value"
            :min="min"
            :max="max"
            :step="step"
            ></b-form-input>
        <div class="d-flex justify-content-center gap-2">
            <b-button @click="set(min)">{{ min }}</b-button>
            <b-button @click="decrement(10 * step)">-{{ 10 * step }}</b-button>
            <b-button @click="decrement(step)">-{{ step }}</b-button>
            <b-button @click="increment(step)">+{{ step }}</b-button>
            <b-button @click="increment(10 * step)">+{{ 10 * step }}</b-button>
            <b-button @click="set(max)">{{ max }}</b-button>
        </div>
    </b-modal>
</template>

<script setup>
import { computed, useTemplateRef, ref } from 'vue';
import { send_command } from '@/js/web_socket.js';
import { useGlobalStore } from '@/stores/global';
import { useSettingsStore } from '@/stores/settings';

const props = defineProps({
    name: {
        type: String,
        required: true,
    },
    min: {
        type: String,
        default: 0,
    },
    max: {
        type: String,
        default: 255,
    },
    step: {
        type: String,
        default: 1,
    },
});

const global = useGlobalStore();
const settings = useSettingsStore()[props.name];

const open_modal = ref(true);
const input_value = ref(parseInt(settings.value, 10) || 0);

const min = parseInt(props.min, 10);
const max = parseInt(props.max, 10);
const step = parseInt(props.step, 10);

const title = computed(() => {
    return `Set ${settings.desc}: ${input_value.value}`;
});

function set(value) {
    if (value < min) {
        input_value.value = min;
    } else if (value > max) {
        input_value.value = max;
    } else {
        input_value.value = value;
    }
    send_command(props.name, input_value.value);
}
function decrement(dec) {
    set(input_value.value - dec);
}
function increment(inc) {
    set(input_value.value + inc);
}

function close_modal() {
    global.closeModal();
}

</script>
