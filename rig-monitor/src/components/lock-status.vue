
<template>
    <div :class="{lock: true, on: on}" @click="toggle">
        LOCK
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { send_command } from '@/js/web_socket.js';
import { useSettingsStore } from '@/stores/settings';

const lock = useSettingsStore().settings.lock;

const on = computed(() => {
    return lock.value;
});

const toggle = (e) => {
    send_command('lock', !lock.value);
}
</script>

<style scoped>
.lock {
    color: lightgrey;
}
.lock.on {
    color: black;
}
</style>
