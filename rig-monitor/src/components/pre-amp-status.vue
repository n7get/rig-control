
<template>
    <div @click="sendClick">{{ pre_amp.value }}</div>
</template>

<script setup>
import { computed } from 'vue';
import { cat_preamp, preamp_cat } from '../js/rig_setting.js';
import { send_command } from '@/js/web_socket.js';
import { useSettingsStore } from '@/stores/settings';

const pre_amp = useSettingsStore().settings.pre_amp;

const sendClick = (e) => {
    let cat = preamp_cat[pre_amp.value];
    cat++;
    if (cat_preamp.hasOwnProperty(cat) === false) {
        cat = 0;
    }
    send_command('pre_amp', cat_preamp[cat]);
}
</script>
