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

</script>
