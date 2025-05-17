<template>
        <b-modal
            id="new-freq"
            size="sm"
            @shown="modal_ready"
            @ok="handle_ok"
            @esc="close_modal"
            @cancel="close_modal"
            v-model="open_modal"
            :title="title"
        >
            <b-form-select
                id="freq-input"
                ref="input_ref"
                v-model="input_value"
                :options="list_options"
                :state="input_state"
            />
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
    list: {
        type: Object,
        required: true,
    },
});

const global = useGlobalStore();
const settings = useSettingsStore()[props.name];
const open_modal = ref(true);
const input_value = ref(settings.value);
const input_state = ref(null);
const input_ref = ref(null);

const title = computed(() => {
    return `Set ${settings.desc}`;
});

const list_options = [];
for (const [v, k] of Object.entries(props.list)) {
    list_options.push({
        value: v,
        text: v,
    });
}

function modal_ready() {
    input_ref.value.focus();
    // input_ref.value.select();
}

const handle_ok = (e) => {
    if(props.list.hasOwnProperty(input_value.value)) {
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
