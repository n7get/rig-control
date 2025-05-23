<template>
    <div class="d-flex flex-wrap justify-content-center gap-2 border border-secondary rounded mt-2 mb-0 pt-2">
        <div :class="['status', { 'on': busy }]">
            BUSY
        </div>
        <div :class="['status', { 'on': hi_swr }]">
            HI SWR
        </div>
        <div :class="['status', { 'on': vfo_a_tx }]">
            VFO A TX
        </div>
        <div :class="['status', { 'on': vfo_a_rx }]">
            VFO A RX
        </div>
        <div :class="['status', { 'on': vfo_b_tx }]">
            VFO B TX
        </div>
        <div :class="['status', { 'on': rec }]">
            REC
        </div>
        <div :class="['status', { 'on': play }]">
            PLAY
        </div>
        <div :class="['status', { 'on': tx_led }]">
            TX LED
        </div>
        <div :class="['status', { 'on': transmit }]">
            TRANSMIT
        </div>
        <div v-if="radio_status === 'MENU MODE'" class="on">
            MENU
        </div>
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { rig_property } from '@/js/rig_property.js';

const busy = computed(() => rig_property('busy').value);
const hi_swr = computed(() => rig_property('hi_swr').value);
const rec = computed(() => rig_property('rec').value);
const play = computed(() => rig_property('play').value);
const vfo_a_tx = computed(() => rig_property('vfo_a_tx').value);
const vfo_b_tx = computed(() => rig_property('vfo_b_tx').value);
const vfo_a_rx = computed(() => rig_property('vfo_a_rx').value);
const tx_led = computed(() => rig_property('tx_led').value);
const transmit = computed(() => rig_property('transmit').value);
const radio_status = computed(() => rig_property('radio_status').value);
</script>

<style scoped>
.status {
    color: white;
}
.status.on {
    color: red;
}
</style>