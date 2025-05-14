<template>
    <div class="mx-auto">
        <div
            @click="freqHandleClicks()"
            class="frequency-digits"
            :class="{tx: transmit}"
        >
            <!-- <span class="mhz">{{ mhz }}</span><span class="khz">{{ khz }}</span><span class="hz">{{ hz }}</span> -->
            {{ mhz }}&nbsp;{{ khz }}&nbsp;{{ hz }}
        </div>

        <b-modal
            id="new-freq"
            size="sm"
            @ok="freqHandleOk"
            @shown="freqFocusInput"
            v-model="freq_model"
            title="Set Frequency"
        >
            <b-form-group
                label="Freq:"
                label-for="freq-input"
                invalid-feedback="Invalid frequency"
                :state="freqInputState"
            >
                <b-form-input
                    id="freq-input"
                    @keyup.enter="freqHandleOk"
                    v-model="freqInput"
                    :state="freqInputState"
                    required
                ></b-form-input>
                <!-- <b-form-input
                    id="freq-input"
                    ref="freqInput"
                    @keyup.enter="freqHandleOk"
                    v-model="freqInput"
                    :state="freqInputState"
                    required
                ></b-form-input> -->
            </b-form-group>
        </b-modal>
    </div>
</template>

<script setup>
import { computed, ref } from 'vue';
import { send_command } from '@/js/web_socket.js';
import freq_utils from '../js/freq_utils.js';
import { useSettingsStore } from '@/stores/settings';
import { useStateStore } from '@/stores/state';

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

const transmit = useSettingsStore().transmit;

const freq_model = ref(false);
const freqInput = ref('');
const freqInputState = ref(null);

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

// Frequency input modal

const freqHandleClicks = (e, v) => {
    freqOpenDialog(v);
}
const freqOpenDialog = (e) => {
    freqResetModal();

    const f = freq_utils.format_freq(freq.value);
    freqInput.value = freq_utils.format_freq(freq.value);

    freq_model.value = true;
}
const freqResetModal = () => {
    freqInputState.value = null;
}

const freqHandleOk = (e) => {
    var new_freq = freq_utils.conv_freq(freqInput.value);

    if(new_freq) {
        freqResetModal();
        send_command(props.vfo, new_freq);
        // this.$nextTick(function() {
        //     this.$bvModal.hide('new-freq');
        // });

        // uibuilder.send({topic: this.vfo, event: 'set', value: new_freq});
    }
    else {
        freqInputState.value = false;
        e.preventDefault();
    }
}

const freqFocusInput = () => {
    // this.$refs.freqInput.focus();
    // this.$refs.freqInput.select();
};

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
