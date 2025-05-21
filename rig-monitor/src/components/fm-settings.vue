<template>
    <settings-component title="FM Settings" :list="list" />
</template>

<script setup>
import settingsComponent from '@/components/settings.vue';

const list = [
    'offset',
    'ctcss',
    'ctcss_tone_frequency',
    'dcs_tone_frequency',
    'mic_gain',
    'squelch_level',
]
</script>