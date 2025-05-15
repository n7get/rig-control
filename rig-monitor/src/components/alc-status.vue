<template>
    <div :class="{'alc-status': true, on: on}">
        ALC
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { useSettingsStore } from '@/stores/settings';

const on = computed(() => {
    return useSettingsStore().settings.read_meter_alc.value;
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
