
<template>
    <div>
        <span @click="open_modal">{{ mode.value }}</span>
    </div>
</template>

<script setup>
import { computed, ref } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { useSettingsStore } from '@/stores/settings';

const mode = useSettingsStore().mode;

const open_modal = (e) => {
    useGlobalStore().openModal('mode');
};
</script>
