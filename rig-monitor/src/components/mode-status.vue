
<template>
    <div>
        <span @click="open_modal">{{ mode.value }}</span>
    </div>
</template>

<script setup>
import { computed, ref } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { rig_property } from '@/js/rig_property.js';

const mode = rig_property('mode');

const open_modal = (e) => {
    useGlobalStore().openModal('mode');
};
</script>
