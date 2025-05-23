<template>
        <b-modal
            focus="freq-input"
            @show="setup_modal"
            @ok="handle_ok"
            @esc="close_modal"
            @cancel="close_modal"
            v-model="freq_model"
            :title="title"
        >
            <b-form-group
                label="Freq:"
                label-for="freq-input"
                invalid-feedback="Invalid frequency"
                :state="freq_input_state"
            >
                <b-form-input
                    id="freq-input"
                    ref="freq_ref"
                    @keyup.enter="handle_ok"
                    v-model="freq_input"
                    :state="freq_input_state"
                    required
                ></b-form-input>
            </b-form-group>
        </b-modal>
</template>

<script setup>
import { computed, useTemplateRef, ref } from 'vue';
import { rig_property } from '@/js/rig_property.js';
import freq_utils from '@/js/freq_utils.js';
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
// const vfo = props.vfo;

const freq_model = ref(true);
const freq_input = ref('');
const freq_input_state = ref(null);

const rp = rig_property(props.vfo);
freq_input.value = rp.value;

const title = computed(() => {
    return `Set ${props.vfo.toUpperCase()} Frequency`;
});

const getFreq = () => {
    let value = rp.value;
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

// Frequency input modal

function setup_modal() {
    const f = freq_utils.format_freq(freq.value);
    freq_input.value = freq_utils.format_freq(freq.value);
}

const handle_ok = (e) => {
    var new_freq = freq_utils.conv_freq(freq_input.value);

    if(new_freq) {
        close_modal();
        rp.update(new_freq);
    }
    else {
        freq_input_state.value = false;
        e.preventDefault();
    }
}

function close_modal() {
    useGlobalStore().closeModal();
    freq_input_state.value = null;
}
</script>

<style>
    .frequency-digits {
        font-family: "Monaco", "Lucida Console", "Andalé Mono";
        font-size: 30px;
        color: green;
        padding-right: 0;
    }
    .frequency-digits.tx {
        color: red;
    }
    .mhz, .khz, .hz {
        display: inline-block;
    }
    .khz, .hz {
        padding-left: 10px;
    }
</style>
