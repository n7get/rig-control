<template>
<div>
    <div class="d-flex justify-content-between">
        <op-mode></op-mode>
        <frequency class="pt-3"></frequency>
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
    <div class="mb-2">
        <div>
        <!-- <s-button event="swap_vfo" label="Swap VFO" class="mt-2"></s-button> -->
        <!-- <s-button event="vfo_a_to_b" label="VFO A > B" class="mt-2"></s-button> -->
        <!-- <s-button event="vfo_b_to_a" label="VFO B > A" class="mt-2"></s-button> -->
        </div>
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

const meter_sw = useSettingsStore().settings.meter_sw;
const mode = useSettingsStore().settings.mode;
const transmit = useSettingsStore().settings.transmit;
</script>

<!-- <script>
module.exports = {
    mixins: [window.noderig.op_mode_mixin],
    components: {
        'alc-meter':        httpVueLoader('alc-meter.vue'),
        'alc-status':       httpVueLoader('alc-status.vue'),
        'comp-meter':        httpVueLoader('comp-meter.vue'),
        'frequency':        httpVueLoader('frequency.vue'),
        'id-meter':         httpVueLoader('id-meter.vue'),
        'lock-status':      httpVueLoader('lock-status.vue'),
        'manual-mode-status': httpVueLoader('manual-mode-status.vue'),
        'mi-level':         httpVueLoader('mi-level.vue'),
        'mode-status':      httpVueLoader('mode-status.vue'),
        'op-mode':          httpVueLoader('op-mode.vue'),
        'po-meter':         httpVueLoader('po-meter.vue'),
        'power-level':      httpVueLoader('power-level.vue'),
        'pre-amp-status':   httpVueLoader('pre-amp-status.vue'),
        's-button':         httpVueLoader('s-button.vue'),
        's-level':          httpVueLoader('s-level.vue'),
        's-meter':          httpVueLoader('s-meter-vue-bootstrap-progressbar.vue'),
//      's-toggle':         httpVueLoader('s-toggle.vue'),
        'swr-meter':        httpVueLoader('swr-meter.vue'),
        'tuner-status':     httpVueLoader('tuner-status.vue'),
        'tx-count-down':    httpVueLoader('tx-count-down.vue'),
        'tx-timer':         httpVueLoader('tx-timer.vue'),
        'vdd-meter':        httpVueLoader('vdd-meter.vue'),
    },
    data: function() {
        return {
            meter_switch: '',
            tx_on: false,
            mode: undefined,
        }
    },
    mounted: function() {
        var self = this;

        uibuilder.onChange('msg', msg => {
            var p = msg.payload;

            if(p.name === 'transmit') {
                if(p.hasOwnProperty('value')) {
                    self.tx_on = p.value;
                }
            }
            else if(p.name === 'meter_switch') {
                if(p.hasOwnProperty('value')) {
                    self.meter_switch = p.value;
                }
            }
            else if(p.name === 'mode') {
                if(p.hasOwnProperty('value')) {
                    self.mode = p.value;
                }
            }
        });
    },
}
</script> -->
