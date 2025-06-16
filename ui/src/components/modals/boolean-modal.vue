<template>
    <b-modal
        focus="form-input"
        @ok="close_modal"
        @esc="close_modal"
        @close="close_modal"
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
import { computed, onBeforeMount, ref } from 'vue';
import { useGlobalStore } from '@/stores/global';

const modal = useGlobalStore().modal;

const open_modal = ref(true);
const input_value = ref(true);

let rp = null;

onBeforeMount(() => {
    rp = modal.rig_prop;
    input_value.value = rp.value;
});

const title = computed(() => {
    return `Set ${rp.desc}`;
});

const set = (e) => {
    useGlobalStore().updateModal(input_value.value);
    close_modal();
}

function close_modal() {
    useGlobalStore().closeModal();
}
</script>
