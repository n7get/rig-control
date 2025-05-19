<template>
        <b-modal
            @ok="close_modal"
            @esc="close_modal"
            v-model="open_modal"
            :title="title"
            ok-only
            ok-title="Close"
        >
            <div class="d-flex justify-content-center">
                <div>
                    <div class="pl-5" v-for="option in list_options" :key="option.value">
                        <b-form-radio
                            v-model="input_value"
                            name="list-options"
                            :value="option.value"
                            @change="select_item(option.value)"
                        >
                            {{ option.text }}
                        </b-form-radio>
                    </div>
                </div>
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
    list: {
        type: Object,
        required: true,
    },
    type: {
        type: String,
        default: 'object',
    },
});

const global = useGlobalStore();
const settings = useSettingsStore()[props.name];
const open_modal = ref(true);
const input_value = ref(settings.value);
const input_ref = ref(null);

const title = computed(() => {
    return `Set ${settings.desc}`;
});

let list_options;
if (props.type === 'list') {
    list_options = props.list.map((k) => ({ value: k, text: k }));
} else {
    list_options = Object.keys(props.list).map((k) => ({ value: k, text: k }));
}

const select_item = (value) => {
    close_modal();
    send_command(props.name, input_value.value);
}

function close_modal() {
    global.closeModal();
}

</script>
