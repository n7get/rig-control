<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Op Modes</b></div>
        <div></div>
    </div>
    <div class="container">
        <b-button class="mb-3 w-100" @click="open('op_mode_import')">Import Op Modes</b-button>
        <b-button class="mb-3 w-100" @click="open('op_mode_export')">Export Op Modes</b-button>
        <b-button class="mb-3 w-100 btn-danger" @click="op_mode_reset">Op Modes Reset</b-button>
    </div>

    <b-modal
        @ok="reset_confirmed"
        @esc="close_confirm_modal"
        @cancel="close_confirm_modal"
        v-model="open_confirm_modal"
        title="Confirm Reset"
        ok-title="Reset"
    >
        <div flush>
            <div>
                You are about to remove all Op Modes. Are you sure you want to continue?
            </div>
        </div>
    </b-modal>
</template>

<script lang="ts" setup>
import { ref } from 'vue';
import { useOpModeStore } from '@/stores/op_modes';
import { op_mode } from '@/js/op_mode.js';
import { useRouter } from 'vue-router';

const router = useRouter();
const open_confirm_modal = ref(false);
const { op_modes } = useOpModeStore();

function op_mode_reset() {
    open_confirm_modal.value = true;
}
function reset_confirmed() {
    Object.values(op_modes)
        .filter((om) => {
            return om.name !== 'Default';
        })
        .forEach((om) => {
            const o = op_mode.fromObject(om);
            console.log(`Removing Op Mode: ${o.name}`);
            o.remove();
        });
    close_confirm_modal();
}
function close_confirm_modal() {
    open_confirm_modal.value = false;
}

function back() {
    router.back();
}

const open = (view) => {
    router.push({ name: view });
};
</script>
