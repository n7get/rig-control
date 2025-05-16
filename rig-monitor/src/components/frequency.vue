<template>
    <div class="mx-auto">
        <div
            @click="freqHandleClicks()"
            class="frequency-digits"
            :class="{'d-flex': true, 'gap-3': true, 'mt-1': true, 'tx': transmit.value}"
        >
            <div class="mhz">{{ mhz }}</div>
            <div class="khz">{{ khz }}</div>
            <div class="hz">{{ hz }}</div>
        </div>
    </div>
</template>

<script setup>
import { computed } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { useSettingsStore } from '@/stores/settings';

const props = defineProps({
    vfo: {
        type: String,
        default: 'vfo_a',
        validator: function (value) {
            return ['vfo_a', 'vfo_b'].indexOf(value) !== -1
        },
    }
});

const transmit = useSettingsStore().transmit;

const getFreq = () => {
    let value = useSettingsStore()[props.vfo].value;
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
    useGlobalStore().openModal(props.vfo)
}
</script>

<style>
    .frequency-digits {
        font-family: "Monaco", "Lucida Console", "Andalé Mono";
        font-size: 40px;
        color: green;
    }
    .frequency-digits.tx {
        color: red;
    }
</style>
