<template>
    <!-- <div v-for="channel in memory_channels" :key="channel.no"> -->
    <div v-for="channel in memory_channels">
        <!-- <div>{{ channel.frequency }}</div> -->
        <div>{{ channel }}</div>
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { useMemoryChannelsStore } from '@/stores/memory_channels.js';

const memory_channels = computed(() => {
    const value = useMemoryChannelsStore().memory_channels;
    return value;
});

console.log('memory_channels', memory_channels);
</script>