<template>
    <div
        @click="freqHandleClicks()"
        class="frequency-digits d-flex justify-content-center align-items-center gap-2 mt-2"
        :class="{'tx': transmit.value}"
        v-if="isReady"
    >
        <div class="mhz">{{ mhz }}</div>
        <div class="khz">{{ khz }}</div>
        <div class="hz">{{ hz }}</div>
    </div>
    <div v-else class="frequency-digits d-flex justify-content-center align-items-center gap-2 mt-2">
        NOT READY
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { rig_property } from '@/js/rig_property.js';
import { useGlobalStore } from '@/stores/global';

const props = defineProps({
    vfo: {
        type: String,
        default: 'vfo_a',
        validator: function (value) {
            return ['vfo_a', 'vfo_b'].indexOf(value) !== -1
        },
    }
});

const vfo = rig_property(props.vfo);
const transmit = rig_property('transmit');
const isReady = computed(() => useGlobalStore().ready);

const getFreq = () => {
    let value = vfo.value;
    if (!value) {
        return '000000000';
    }
    
    value = value.toString();
    while(value.length < 9) {
        value = ' ' + value;
    }

    return value;
}

const freq = computed(() => {
    return getFreq();
});

const mhz = computed(() => {
    const f = getFreq();

    return f.substring(0, 3);
})
const khz = computed(() => {
    const f = getFreq();

    return f.substring(3, 6);
});
const hz = computed(() => {
    const f = getFreq();

    return f.substring(6, 9);
});

const freqHandleClicks = () => {
    useGlobalStore().openModal(vfo, (value) => {
        vfo.update(value);
    });
}
</script>

<style>
    .frequency-digits {
        font-family: "Monaco", "Lucida Console", "Andalé Mono";
        font-size: 36px !important;
        color: green;
    }
    .frequency-digits.tx {
        color: red;
    }
</style>
