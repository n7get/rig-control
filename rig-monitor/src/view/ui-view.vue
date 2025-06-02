<template>
    <div>
        <div class="col-sm-12 col-md-6 col-lg-4 px-0">
            <b-container fluid>
                <b-row class="mt-2">
                    <b-col class="col-3">
                        <b-button cols="9" id="op-mode-button" @click="open_op_modes">{{ name }}</b-button>                        
                    </b-col>
                    <b-col><frequency /></b-col>
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
import { computed } from 'vue';
import { useOpModeStore } from '@/stores/op_modes';
import { useRouter } from 'vue-router';

import frequency from '@/components/frequency.vue';
import controls from '@/components/controls.vue'
import groups from '@/components/groups.vue';
// import radioStatus from '@/components/radio_status.vue'
import settingsModal from '@/components/modals/settings-modal.vue';

const router = useRouter();

const name = computed(() => {
    const om_ref = useOpModeStore().get_current_op_mode;
    return om_ref ? om_ref.name : 'Op Mode';
});

const open_op_modes = () => {
    console.log('router: ', router);
    router.push({ name: 'op_modes' });
}

const handleOk = (e) => {
    // uibuilder.send({topic: 'rig_status', event: 'enable_transmit', value: true});
}
</script>

<style scoped>
    #op-mode-button {
        height: 64px;
        width: 80px;
    }
</style>
