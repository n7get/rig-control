<template>
        <b-modal
            id="new-freq"
            size="sm"
            @show="setup_modal"
            @shown="modal_ready"
            @ok="handle_ok"
            @cancel="close_modal"
            v-model="open_modal"
            :title="title"
        >
                <b-form-input
                    id="freq-input"
                    ref="input_ref"
                    @keyup.enter="handle_ok"
                    v-model="input_value"
                    :state="input_state"
                    required
                ></b-form-input>
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
        type: Number,
        default: 0,
    },
    max: {
        type: Number,
        default: 255,
    },
    scale: {
        type: Number,
        default: 1,
    },
});

const global = useGlobalStore();
const settings = useSettingsStore()[props.name];
const open_modal = ref(true);
const input_value = ref('');
const input_state = ref(null);
const input_ref = ref(null);

const title = computed(() => {
    return `Set ${settings.desc}`;
});

function setup_modal() {
    input_value.value = settings.value
    input_state.value = null;
}

function modal_ready() {
    input_ref.value.focus();
    // input_ref.value.select();
}

const handle_ok = (e) => {
    const number = parseInt(input_value.value, 10);

    if(number >= props.min && number <= props.max) {
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
