<template>
    <b-modal
        focus="form-input"
        @ok="close_modal"
        @esc="close_modal"
        v-model="open_modal"
        :title="title"
        ok-only
        ok-title="Close"
    >
        <b-form-input
            id="form-input"
            type="range"
            @change="set(input_value)"
            v-model="input_value"
            :min="min"
            :max="max"
            :step="step"
            ></b-form-input>
        <div class="d-flex justify-content-center gap-1">
            <b-button @click="set(min)">{{ min }}</b-button>
            <b-button @click="decrement(fast_step)">-{{ fast_step }}</b-button>
            <b-button @click="decrement(step)">-{{ step }}</b-button>
            <b-button @click="increment(step)">+{{ step }}</b-button>
            <b-button @click="increment(fast_step)">+{{ fast_step }}</b-button>
            <b-button @click="set(max)">{{ max }}</b-button>
        </div>
    </b-modal>
</template>

<script setup>
import { computed, onBeforeMount, ref } from 'vue';
import { useGlobalStore } from '@/stores/global';

const modal = useGlobalStore().modal;

let rp = null;
const open_modal = ref(true);
const input_value = ref(0);

let min = 0;
let max = 0;
let step = 0;
let fast_step = 0;

onBeforeMount(() => {
    rp = modal.rig_prop;
    input_value.value = rp.value;

    const range = rp.range || {};
    min = range.hasOwnProperty('min') ? range.min : 0;
    max = range.hasOwnProperty('max') ? range.max : 100;
    step = range.hasOwnProperty('step') ? range.step : 1;
    fast_step = range.hasOwnProperty('fast_step') ? range.fast_step : 10;
});

const title = computed(() => {
    return `Set ${rp.desc}: ${input_value.value}${rp.suffix}`;
});

function set(value) {
    if (value < min) {
        input_value.value = min;
    } else if (value > max) {
        input_value.value = max;
    } else {
        input_value.value = value;
    }
    useGlobalStore().updateModal(input_value.value);
}
function decrement(dec) {
    set(input_value.value - dec);
}
function increment(inc) {
    set(input_value.value + inc);
}

function close_modal() {
    useGlobalStore().closeModal();
}

</script>
