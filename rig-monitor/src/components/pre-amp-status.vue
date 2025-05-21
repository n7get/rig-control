
<template>
    <div @click="open_modal">{{ pre_amp.value }}</div>
</template>

<script setup>
import { computed } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { rig_property } from '@/js/rig_property.js';

const pre_amp = rig_property('pre_amp');

const open_modal = () => {
    useGlobalStore().openModal('pre_amp');
};
</script>
