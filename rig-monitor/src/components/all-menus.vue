<template>
    <settings-component title="All Menus" :list="list" open="true" />
</template>

<script setup>
import settingsComponent from '@/components/settings.vue';
import { get_all_menus_list } from '@/js/rig_property.js';

const list = get_all_menus_list();
</script>
