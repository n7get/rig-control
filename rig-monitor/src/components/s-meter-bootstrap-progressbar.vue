<template>
    <!-- <div v-show="!tx_on"> -->
    <div>
        <div>
            S-Meter:
        </div>
        <div class="progress">
            <div class="progress">
                <div
                    class="progress-bar"
                    role="progressbar"
                    :style="{ width: value }"
                    aria-valuenow="50"
                    aria-valuemin="0"
                    aria-valuemax="100"
                ></div>
            </div>
        </div>
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { rig_property } from '@/js/rig_property.js';

const s_meter = rig_property('s_meter_reading');
const tx_on = rig_property('transmit');
const value = computed(() => {
    return s_meter.value + '%';
});
</script>

<style scoped>
    .progress {
	    width: 100%;
    }
    .progress-bar {
        color: red;
    }
</style>
