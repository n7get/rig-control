<template>
    <b-modal
        @ok="close_modal"
        @esc="close_modal"
        v-model="open_modal"
        :title="title"
    >
        <template #footer="{ cancel }">
            <div class="d-flex justify-content-between w-100">
                <command-selector :name="props.name" />
                <b-button variant="secondary" @click="cancel">Done</b-button>
            </div>
        </template>
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
import { useGlobalStore } from '@/stores/global';
import { rig_property } from '@/js/rig_property.js';

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

const open_modal = ref(true);
const input_value = ref('');
const rp = rig_property(props.name);
input_value.value = rp.value;

const title = computed(() => {
    return `Set ${rp.desc}`;
});

let list_options = props.list.map((k) => ({ value: k, text: k }));

const select_item = (value) => {
    close_modal();
    rp.update(input_value.value);
}

function close_modal() {
    useGlobalStore().closeModal();
}

let target_op_mode = ref('');
function delete_op_mode(name) {
    target_op_mode.value = name;
    open_confirm_modal.value = true;
}
function delete_confirmed() {
    send_message({ topic: 'op_mode', event: 'delete', value: { name: target_op_mode.value }});
    close_confirm_modal();
}

function add_op_mode() {
}
function edit_op_mode(name) {
}
</script>
