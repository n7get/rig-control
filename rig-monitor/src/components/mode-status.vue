
<template>
    <div>
        <span @click="open_modal">{{ mode.value }}</span>

        <b-modal
            id="new-mode"
            size="sm"
            hide-footer
            v-model="show_modal"
        >
            <template #modal-title>
                Set mode
            </template>
            <div class="font-weight-bold my-1 ml-0 py-3 pl-3 border text-light bg-primary" v-for="m in mode_options" @click="handle_click(m)">&nbsp;&nbsp;{{ m }}</div>
        </b-modal>
    </div>
</template>

<script setup>
import { computed, ref } from 'vue';
import { modes_cat } from '../js/rig_setting.js';
import { send_command } from '@/js/web_socket.js';
import { useSettingsStore } from '@/stores/settings';

const mode = useSettingsStore().mode;
const mode_options = computed(() => {
    return Object.keys(modes_cat).map(m => {
        return m;
    });
});

const show_modal = ref(false);
const handle_click = (m) => {
    show_modal.value = false;
    send_command('mode', m);
};
const open_modal = (e) => {
    show_modal.value = true;
};
</script>
