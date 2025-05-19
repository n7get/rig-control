<template>
        <b-modal
            focus="form-input"
            @ok="close_modal"
            @esc="close_modal"
            @cancel="close_modal"
            v-model="open_modal"
            :title="title"
            ok-only
            ok-title="Close"
        >
            <div class="d-flex gap-3 justify-content-center">
                <b-form-checkbox
                    id="form-input"
                    @change="set"
                    reverse switch
                    size="lg"
                    v-model="input_value"
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

const title = computed(() => {
    return `Set ${settings.desc}`;
});

const set = (e) => {
    send_command(props.name, input_value.value);
}

function close_modal() {
    global.closeModal();
}
</script>
