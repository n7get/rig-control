<template v-if="modal.open">
    <number-modal name="af_gain" v-if="modal.name === 'af_gain'" />
    <list-modal name="agc" :list="agc_to_cat" v-if="modal.name === 'agc'" />
    <list-modal name="ctcss" :list="ctcss_to_cat" v-if="modal.name === 'ctcss'" />
    <list-modal name="offset" :list="offset_cat" v-if="modal.name === 'offset'" />
    <list-modal name="ctcss_tone_frequency" :list="ctcss_tone_frequency_to_cat" v-if="modal.name === 'ctcss_tone_frequency'" />
    <list-modal name="dcs_tone_frequency" :list="cat_to_dcs" type="list" v-if="modal.name === 'dcs_tone_frequency'" />
    <number-modal name="if_shift" min="-1200" max="1200" step="20" v-if="modal.name === 'if_shift'" />
    <number-modal name ="key_pitch" min="300" max="1050" step="10" v-if="modal.name === 'key_pitch'" />
    <number-modal name="key_speed" min="4" max="60" step="1" v-if="modal.name === 'key_speed'" />
    <list-modal name="meter_sw" :list="meter_cat" v-if="modal.name === 'meter_sw'" />
    <number-modal name="mic_gain" min="0" max="100" step="1" v-if="modal.name === 'mic_gain'" />
    <list-modal name="mode" :list="modes_cat" v-if="modal.name === 'mode'" />
    <number-modal name="manual_notch_level" min="10" max="3200" step="10" v-if="modal.name === 'manual_notch_level'" />
    <number-modal name="monitor_level" min="0" max="100" step="1" v-if="modal.name === 'monitor_level'" />
    <number-modal name="noise_blanker_level" min="0" max="10" step="1" v-if="modal.name === 'noise_blanker_level'" />
    <number-modal name="noise_reduction_level" min="1" max="15" step="1" v-if="modal.name === 'noise_reduction_level'" />
    <number-modal name="power_level" min="5" max="100" step="1" v-if="modal.name === 'power_level'" />
    <list-modal name="pre_amp" :list="preamp_cat" v-if="modal.name === 'pre_amp'" />
    <number-modal name="rf_gain" v-if="modal.name === 'rf_gain'" />
    <number-modal name="speech_processor_level" min="0" max="100" step="1" v-if="modal.name === 'speech_processor_level'" />
    <number-modal name="squelch_level" min="0" max="100" step="1" v-if="modal.name === 'squelch_level'" />
    <vfo-modal vfo="vfo_a" v-if="modal.name === 'vfo_a'" />
    <vfo-modal vfo="vfo_b" v-if="modal.name === 'vfo_b'" />
    <number-modal name="vox_gain" min="0" max="100" step="1" v-if="modal.name === 'vox_gain'" />

    <repeater-modal v-if="modal.name === 'repeater'" />
</template>

<script setup>
import { useGlobalStore } from '@/stores/global';
import {
    agc_to_cat,
    ctcss_to_cat,
    ctcss_tone_frequency_to_cat,
    meter_cat,
    modes_cat,
    cat_to_dcs,
    offset_cat,
    preamp_cat,
} from '@/js/rig_setting.js';

import booleanModal from './boolean_modal.vue';
import listModal from './list_modal.vue';
import numberModal from './number_modal.vue';
import vfoModal from './vfo_modal.vue';

import repeaterModal from './repeater_modal.vue';

const modal = useGlobalStore().modal;
</script>
