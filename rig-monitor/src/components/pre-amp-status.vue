
<template>
    <div @click="open_modal">{{ pre_amp.value }}</div>
</template>

<script setup>
import { computed } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { useSettingsStore } from '@/stores/settings';

const pre_amp = useSettingsStore().pre_amp;

const open_modal = () => {
    useGlobalStore().openModal('pre_amp');
};
</script>
