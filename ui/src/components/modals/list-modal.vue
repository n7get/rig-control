<template>
    <b-modal
        @ok="close_modal"
        @esc="close_modal"
        @close="close_modal"
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
import { computed, onBeforeMount, ref } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { rig_property } from '@/js/rig_property.js';

const modal = useGlobalStore().modal;

const open_modal = ref(true);
const input_value = ref('');
let rp = null;
let list_options = [];

onBeforeMount(() => {
    rp = modal.rig_prop;
    input_value.value = rp.value;
    list_options = rp.list.map((k) => ({ value: k, text: k }));
});

const title = computed(() => {
    return `Set ${rp.desc}`;
});

const select_item = (value) => {
    useGlobalStore().updateModal(input_value.value);
    close_modal();
}

function close_modal() {
    useGlobalStore().closeModal();
}
</script>
