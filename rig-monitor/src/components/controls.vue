<template>
    <s-meter meter="s_meter_reading" label="S-Meter" v-show="!transmit.value" class="mt-2" />
    <s-meter meter="read_meter_swr" label="SWR" v-show="transmit.value" class="mt-2" />
    <s-meter meter="read_meter_alc" label="ALC" v-show="transmit.value" class="mt-2" />
    <div v-show="transmit.value" class="mt-2">
        <s-meter meter="read_meter_front_panel" label="COMP" v-show="meter_sw.value === 'COMP'" />
        <s-meter meter="read_meter_front_panel" label="ID" v-show="meter_sw.value === 'ID'" />
        <s-meter meter="read_meter_front_panel" label="PO" v-show="meter_sw.value === 'PO'" />
        <s-meter meter="read_meter_front_panel" label="VDD" v-show="meter_sw.value === 'VDD'" />
    </div>
    <!-- <tx-timer class="mt-2" /> -->
    <!-- <tx-count-down :warn="30" /> -->
    <power-level class="mt-3" />
    <s-level 
        v-show="mode.value === 'FM' || mode.value === 'C4FM' || mode.value === 'LSB' || mode.value === 'USB'"
        label="Mic-G"
        event="mic_gain"
        class="mt-3" />
    <s-level
        v-show="mode.value === 'FM' || mode.value === 'C4FM'"
        label="SQ"
        event="squelch_level"
        class="mt-2" />
    <s-level-toggle
        v-show="mode.value === 'DATA-LSB' || mode.value === 'DATA-USB'"
        label="Mon"
        event="monitor_level"
        toggle="monitor"
        class="mt-2" />
    <s-level-toggle
        v-show="mode.value === 'LSB' || mode.value === 'USB'"
        label="NB"
        event="noise_blanker_level"
        toggle="noise_blanker"
        :max="10"
        class="mt-2" />
    <s-level-toggle
        v-show="mode.value === 'LSB' || mode.value === 'USB'"
        label="NR"
        event="noise_reduction_level"
        toggle="noise_reduction"
        :min="1"
        :max="15"
        class="mt-2" />
    <s-level
        v-show="mode.value === 'DATA-USB'"
        event="73"
        label="D-LVL"
        class="mt-2" />
    <s-level
        v-show="mode.value === 'DATA-FM'"
        event="75"
        label="FM OUT"
        class="mt-2" />
    <s-level
        v-show="mode.value === 'DATA-FM'"
        event="78"
        label="FM TX GAIN"
        class="mt-2" />
    <div class="d-flex justify-content-center gap-2 mb-2">
        <s-button event="swap_vfo" label="Swap VFO" class="mt-2" />
        <s-button event="vfo_a_to_vfo_b" label="VFO A > B" class="mt-2" />
        <s-button event="vfo_b_to_vfo_a" label="VFO B > A" class="mt-2"></s-button>
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { rig_property } from '@/js/rig_property.js';

import sMeter from '@/components/s-meter.vue';
import powerLevel from '@/components/power-level.vue';
import sLevel from '@/components/s-level.vue';
import sLevelToggle from '@/components/s-level-toggle.vue';
import sButton from '@/components/s-button.vue';

const meter_sw = rig_property('meter_sw');
const mode = rig_property('mode');
const transmit = rig_property('transmit');
</script>
