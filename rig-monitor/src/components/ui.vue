<template>
    <div>
        <div class="col-sm-12 col-md-6 col-lg-4 px-0">
            <b-container fluid>
                <b-row class="mt-2">
                    <b-col class="col-3"><op-mode /></b-col>
                    <b-col class="col-9"><frequency /></b-col>
                </b-row>
                <b-row class="mt-2">
                    <b-col><tuner-status /></b-col>
                    <b-col><lock-status /></b-col>
                    <b-col><pre-amp-status /></b-col>
                    <b-col><alc-status /></b-col>
                    <b-col><mode-status /></b-col>
                    <!-- <manual-mode-status /> -->
                </b-row>
                <controls />
                <!-- <radio-status class="px-3" /> -->
                <!-- <rbn /> -->
                <!-- <config /> -->
            </b-container>
            <groups />
        </div>
  
        <settings-modal />

        <b-modal
            id="alert"
            size="sm"
            @ok="handleOk"
            no-close-on-esc
            no-close-on-backdrop
            ok-only
            hide-header-close
            header-bg-variant="danger"
            header-text-variant="light"
        >
            <template #modal-title>
                High SWR
            </template>
            <div>Transmit is disabled until OK is clicked.</div>
        </b-modal>
    </div>
</template>

<script setup>
import { useGlobalStore } from '@/stores/global';

import frequency from './frequency.vue';
import opMode from './op-mode.vue';
import controls from '@/components/controls.vue'
import groups from '@/components/groups.vue';
// import radioStatus from '@/components/radio_status.vue'
import settingsModal from '@/components/modals/settings-modal.vue';

const handleOk = (e) => {
    // uibuilder.send({topic: 'rig_status', event: 'enable_transmit', value: true});
}
</script>
