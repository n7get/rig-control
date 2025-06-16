<template>
    <div :class="{'alc-status': true, on: on}">
        ALC
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { rig_property } from '@/js/rig_property.js';

const on = computed(() => {
    return rig_property('read_meter_alc').value;
});
</script>

<style scoped>
.alc-status {
    color: lightgrey;
}
.alc-status.on {
    color: black;
}
</style>
