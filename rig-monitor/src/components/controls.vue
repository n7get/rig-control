<template>
<div>
    <div class="d-flex justify-content-between mt-4">
        <op-mode></op-mode>
        <frequency></frequency>
    </div>
    <!-- <b-row v-show="isOpMode(['data-hf', 'voice-ssh'])" class="mt-2"> -->
    <div class="ml-1 my-2 pb-1 border-bottom d-flex justify-content-between w-100">
        <tuner-status></tuner-status>
        <lock-status>></lock-status>
        <pre-amp-status>></pre-amp-status>
        <alc-status>></alc-status>
        <mode-status></mode-status>
        <!-- <manual-mode-status></manual-mode-status> -->
    </div>
    <s-meter v-show="!transmit.value" class="mt-2"></s-meter>
    <swr-meter v-show="transmit.value" class="mt-2"></swr-meter>
    <alc-meter v-show="transmit.value" class="mt-2"></alc-meter>
    <div v-show="transmit.value" class="mt-2">
        <read-meter-front-panel meter="COMP" v-show="meter_sw.value === 'COMP'"></read-meter-front-panel>
        <read-meter-front-panel meter="ID" v-show="meter_sw.value === 'ID'"></read-meter-front-panel>
        <read-meter-front-panel meter="PO" v-show="meter_sw.value === 'PO'"></read-meter-front-panel>
        <read-meter-front-panel meter="VDD" v-show="meter_sw.value === 'VDD'"></read-meter-front-panel>
    </div>
    <!-- <tx-timer class="mt-2"></tx-timer> -->
    <!-- <tx-count-down :warn="30"></tx-count-down> -->
    <power-level class="mt-2 mb-2"></power-level>
    <s-level 
        v-show="mode.value === 'FM' || mode.value === 'C4FM' || mode.value === 'LSB' || mode.value === 'USB'"
        label="Mic-G"
        event="mic_gain"
        class="mt-2 mb-2"></s-level>
    <s-level
        v-show="mode.value === 'FM' || mode.value === 'C4FM'"
        label="SQ"
        event="squelch_level"
        class="mt-2 mb-2"></s-level>
    <s-level
        v-show="mode.value === 'DATA-LSB' || mode.value === 'DATA-USB'"
        label="Mon"
        event="monitor_level"
        toggle="monitor"
        class="mt-2 mb-2"></s-level>
    <s-level
        v-show="mode.value === 'LSB' || mode.value === 'USB'"
        label="NB"
        event="noise_blanker_level"
        toggle="noise_blanker_status"
        :max="10"
        class="mt-2 mb-2"></s-level>
    <s-level
        v-show="mode.value === 'LSB' || mode.value === 'USB'"
        label="NR"
        event="noise_reduction_level"
        toggle="noise_reduction"
        :min="1"
        :max="15"
        class="mt-2 mb-2"></s-level>
    <mi-level
        v-show="mode.value === 'DATA-USB'"
        :no="73"
        label="D-LVL"
        class="mt-2 mb-2"></mi-level>
    <mi-level
        v-show="mode.value === 'DATA-FM'"
        :no="75"
        label="FM OUT"
        class="mt-2 mb-2"></mi-level>
    <mi-level
        v-show="mode.value === 'DATA-FM'"
        :no="78"
        label="FM TX GAIN"
        class="mt-2 mb-2"></mi-level>
    <div class="d-flex gap-2 mb-2">
        <s-button event="swap_vfo" label="Swap VFO" class="mt-2"></s-button>
        <s-button event="vfo_a_to_vfo_b" label="VFO A > B" class="mt-2"></s-button>
        <s-button event="vfo_b_to_vfo_a" label="VFO B > A" class="mt-2"></s-button>
    </div>
</div>
</template>

<script setup>
import { computed } from 'vue';
import { useSettingsStore } from '@/stores/settings';

import frequency from './frequency.vue';
import opMode from './op-mode.vue';
import tunerStatus from './tuner-status.vue';
import lockStatus from './lock-status.vue';
import preAmpStatus from './pre-amp-status.vue';
import alcStatus from './alc-status.vue';
import modeStatus from './mode-status.vue';
import sMeter from './s-meter-bootstrap-progressbar.vue';
import swrMeter from './swr-meter.vue';
import alcMeter from './alc-meter.vue';
import readMeterFrontPanel from './read-meter-front-panel.vue';
import powerLevel from './power-level.vue';
import sLevel from './s-level.vue';
import sButton from './s-button.vue';

const meter_sw = useSettingsStore().meter_sw;
const mode = useSettingsStore().mode;
const transmit = useSettingsStore().transmit;
</script>
