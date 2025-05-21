
<template>
    <div :class="{lock: true, on: on}" @click="toggle">
        LOCK
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { rig_property } from '@/js/rig_property.js';

const lock = rig_property('lock');

const on = computed(() => {
    return lock.value;
});

const toggle = (e) => {
    lock.update(!lock.value);
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
