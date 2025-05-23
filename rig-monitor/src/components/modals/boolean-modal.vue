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
                >{{ rp.desc }}</b-form-checkbox>
            </div>
        </b-modal>
</template>

<script setup>
import { computed, useTemplateRef, ref } from 'vue';
import { rig_property } from '@/js/rig_property.js';
import { useGlobalStore } from '@/stores/global';

const props = defineProps({
    name: {
        type: String,
        required: true,
    },
});

const open_modal = ref(true);
const input_value = ref(true);

const rp = rig_property(props.name);
input_value.value = rp.value;

const title = computed(() => {
    return `Set ${rp.desc}`;
});

const set = (e) => {
    rp.update(input_value.value);
}

function close_modal() {
    useGlobalStore().closeModal();
}
</script>
