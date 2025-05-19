
<template>
    <div :class="{tuner: true, on: on}" @click="sendClick">
        TUNER
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { send_command } from '@/js/web_socket.js';
import { useSettingsStore } from '@/stores/settings';

const tuner = useSettingsStore().antenna_tuner_control;

const on = computed(() => {
    return tuner.value === 'on';
});

const sendClick = (e) => {
    console.log('sendClick: ', e);
    send_command('antenna_tuner_control', tuner.value === 'on' ? 'off' : 'on');
}
</script>

<style scoped>
.tuner {
    color: lightgrey;
}
.tuner.on {
    color: black;
}
</style>
