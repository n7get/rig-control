<template>
    <b-button id="op-mode-button">{{ name }}</b-button>
</template>

<script setup>
import { ref } from 'vue';

const name = ref('OP Mode');
</script>

<style scoped>
    #op-mode-button {
        height: 64px;
        width: 80px;
    }
</style>
