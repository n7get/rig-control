<template>
    <b-modal
        id="new-freq"
        size="sm"
        @show="setup_modal"
        @shown="modal_ready"
        @ok="handle_ok"
        @esc="close_modal"
        @cancel="close_modal"
        v-model="open_modal"
        :title="title"
    >
        <b-form-input
            id="freq-input"
            type="range"
            ref="input_ref"
            @keyup.enter="handle_ok"
            v-model="input_value"
            :state="input_state"
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
const input_value = ref(0);
const input_state = ref(null);
const input_ref = ref(null);

const min = parseInt(props.min, 10);
const max = parseInt(props.max, 10);
const step = parseInt(props.step, 10);

const title = computed(() => {
    return `Set ${settings.desc}: ${input_value.value}`;
});

function set(value) {
    if (value < min || value > max) {
        input_state.value = false;
    } else {
        input_value.value = value;
        input_state.value = true;
    }
}
function decrement(dec) {
    if (isNaN(input_value.value)) {
        input_value.value = min;
    } else {
        input_value.value = Math.max(min, input_value.value - dec);
    }
    if (input_value.value < min) {
        input_value.value = min;
    }
}
function increment(inc) {
    if (isNaN(input_value.value)) {
        input_value.value = min;
    } else {
        input_value.value = Math.min(max, input_value.value + inc);
    }
    if (input_value.value > max) {
        input_value.value = max;
    }
}
function setup_modal() {
    input_value.value = parseInt(settings.value, 10);
    input_state.value = null;
}

function modal_ready() {
    input_ref.value.focus();
    // input_ref.value.select();
}

const handle_ok = (e) => {
    const number = parseInt(input_value.value, 10);

    if(number >= min && number <= max) {
        close_modal();
        send_command(props.name, input_value.value);
    }
    else {
        input_state.value = false;
        e.preventDefault();
    }
}

function close_modal() {
    global.closeModal();
    input_state.value = null;
}

</script>
