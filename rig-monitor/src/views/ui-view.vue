<template>
    <div>
        <div class="col-sm-12 col-md-6 col-lg-4 px-0">
            <div class="m-3 pb-2 d-flex justify-content-between border-bottom">
                <div @click="open_config"><u>Config</u></div>
                <div><b>FT-991A</b></div>
                <div @click="open_mem_chan"><u>Mem chans</u></div>
            </div>
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

const router = useRouter();

const name = computed(() => {
    const om_ref = useOpModeStore().get_current_op_mode;
    return om_ref ? om_ref.name : 'Op Mode';
});

const open_op_modes = () => {
    console.log('router: ', router);
    router.push({ name: 'op_modes' });
}

const open_config = () => {
    router.push({ name: 'config' });
}
const open_mem_chan = () => {
    router.push({ name: 'mem_chans' });
}
</script>

<style scoped>
    #op-mode-button {
        height: 64px;
        width: 80px;
    }
</style>
