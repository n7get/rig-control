<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Config</b></div>
        <div></div>
    </div>
    <div class="container">
        <b-button class="w-100 mb-3" @click="open('group_config')">Groups</b-button>
        <b-button class="w-100 mb-3" @click="open('mem_chan_config')">Memory Channels</b-button>
        <b-button class="w-100 mb-3" @click="open('op_mode_config')">Op Modes</b-button>
        <b-button class="w-100 mb-3" @click="open('wifi_config')">Wifi Config</b-button>
        <b-button class="w-100 mb-3" @click="open('info')">Info</b-button>
    </div>
</template>

<script lang="ts" setup>
import { useRouter } from 'vue-router';

const router = useRouter();

function back() {
    router.back();
}

const open = (view) => {
    router.push({ name: view });
};
</script>
