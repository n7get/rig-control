<template v-if="modal.open">
    <number-modal name="af_gain" v-if="modal.name === 'af_gain'" />
    <list-modal name="agc" :list="agc_to_cat" v-if="modal.name === 'agc'" />
    <number-modal name="rf_gain" v-if="modal.name === 'rf_gain'" />
    <vfo-modal vfo="vfo_a" v-if="modal.name === 'vfo_a'" />
    <vfo-modal vfo="vfo_b" v-if="modal.name === 'vfo_b'" />
</template>

<script setup>
import { useGlobalStore } from '@/stores/global';
import { agc_to_cat } from '@/js/rig_setting.js';

import listModal from './list_modal.vue';
import numberModal from './number_modal.vue';
import vfoModal from './vfo_modal.vue';

const modal = useGlobalStore().modal;
</script>
