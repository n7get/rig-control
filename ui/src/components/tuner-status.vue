
<template>
    <div :class="{tuner: true, on: on}" @click="send_update">
        TUNER
    </div>
</template>

<script setup>
import { computed, ref } from 'vue';
import { rig_property } from '@/js/rig_property.js';

const antenna_tuner_control = rig_property('antenna_tuner_control');

const on = computed(() => {
    return antenna_tuner_control.value === 'on';
});

const send_update = (e) => {
    antenna_tuner_control.update(antenna_tuner_control.value === 'on' ? 'off' : 'on');
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
