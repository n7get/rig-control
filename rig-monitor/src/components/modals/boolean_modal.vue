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
            <div class="d-flex gap-3 justify-content-center">
                <b-form-checkbox
                    reverse switch
                    size="lg"
                    id="freq-input"
                    ref="input_ref"
                    v-model="input_value"
                    :state="input_state"
                >{{ settings.desc }}</b-form-checkbox>
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

function modal_ready() {
    input_ref.value.focus();
    // input_ref.value.select();
}

const handle_ok = (e) => {
    send_command(props.name, input_value.value);
}

function close_modal() {
    global.closeModal();
}

</script>
