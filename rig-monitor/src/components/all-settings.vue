<template>
    <settings-component title="All Settings" :list="list" open="false" />
</template>

<script setup>
import settingsComponent from '@/components/settings.vue';
import { get_all_settings_list } from '@/js/rig_property.js';

const list = get_all_settings_list()
</script>
