<template>
    <div>
        <div class="m-3 pb-2 d-flex justify-content-between border-bottom">
            <div @click="cancel"><u>Cancel</u></div>
            <div><b>EDIT OP MODE</b></div>
            <div @click="save_op_mode"><u>Save</u></div>
        </div>
        <div class="m-3">
            <b-form-group
                class="mt-2"
                label="Name:"
                label-for="name"
            >
                <b-form-input
                    id="name"
                    ref="name-input"
                    v-model.lazy="om.name"
                    :state="validate('name')"
                    required
                ></b-form-input>
                <div v-if="errors('name')" class="text-danger">{{ errors('name') }}</div>
            </b-form-group>

            <b-form-group
                class="mt-2"
                label="Order:"
                label-for="order"
            >
                <b-form-input
                    id="order"
                    ref="order-input"
                    type="number"
                    v-model.lazy="om.order"
                    :state="validate('order')"
                    required
                ></b-form-input>
                <div v-if="errors('order')" class="text-danger">{{ errors('order') }}</div>
            </b-form-group>

            <b-form-group
                label="Frequency Ranges:"
                label-for="frequency-ranges"
                label-class="mt-2"
                :state="fr_fg_state"
            >
                <div v-for="(range, index) in om.freq_ranges" :key="index" class="mb-2">
                    <div class="d-flex gap-2">
                        <b-form-input
                            v-model.lazy="om.freq_ranges[index].start"
                            type="number"
                            placeholder="Start Frequency"
                            :state="fr_state(index, 0)"
                        ></b-form-input>
                        <b-form-input
                            v-model.lazy="om.freq_ranges[index].end"
                            type="number"
                            placeholder="End Frequency"
                            :state="fr_state(index, 1)"
                        ></b-form-input>
                        <b-button variant="danger" @click="removeFrequencyRange(index)"><IBiXLg /></b-button>
                    </div>
                    <div v-if="errors('freq_ranges', index)" class="text-danger"><i>{{ errors('freq_ranges', index) }}</i></div>
                </div>
                <div v-if="at_max_freq_ranges">
                    <div class="d-flex gap-2 mb-2">
                    <b-form-input
                        v-model.lazy="freq_ranges_start"
                        type="number"
                        placeholder="Start Frequency"
                        :state="null"
                        required
                    ></b-form-input>
                    <b-form-input
                        v-model.lazy="freq_ranges_end"
                        type="number"
                        placeholder="End Frequency"
                        :state="null"
                        required
                    ></b-form-input>
                    <b-button variant="primary" @click="addFrequencyRange()"><IBiPlus /></b-button>
                    </div>
                    <div v-if="freq_inputs_error" class="text-danger"><i>{{ freq_inputs_error }}</i></div>
                </div>
                <div v-if="props.edit != 'true'" class="mt-4 border-top pt-4">
                    <b-form-textarea
                    id="textarea"
                    v-model="op_mode_text"
                    placeholder="Pase Op Mode JSON text here..."
                    rows="6"
                    max-rows="12"
                    ></b-form-textarea>
                    <b-button class="mt-2" variant="primary" @click="parse_json">Parse JSON</b-button>
                </div>
            </b-form-group>
        </div>
    </div>
</template>

<script setup>
import { computed, watchEffect, ref } from 'vue';
import { useOpModeStore } from '@/stores/op_modes';
import IBiPlus from '~icons/bi/plus';
import IBiXLg from '~icons/bi/x-lg';
import { useRouter } from 'vue-router';
import { config, freq_range, op_mode } from '@/js/op_mode';
import { conv_freq, is_in_band } from '@/js/freq_utils';

const router = useRouter();

const props = defineProps({
    id: {
        type: String,
    },
    edit: {
        type: String,
        required: true,
    },
});

const om = ref({});
const freq_ranges_start = ref('');
const freq_ranges_end = ref('');
const op_mode_text = ref('');

const name_input_state = ref(true);
const order_input_state = ref(true);
const fr_fg_state = ref(true);

let error_messages = {};
function validate(field) {
    const o = op_mode.fromObject(om.value);
    error_messages = o.validate(field);
    console.log('Validation errors:', error_messages);
}

const errors = (field, index = null) => {
    switch (field) {
        case 'name':
            return error_messages.name || null;
        case 'order':
            return error_messages.order || null;
        case 'freq_ranges':
            const messages = error_messages.freq_ranges || [];
            if (index !== null && messages[index]) {
                return messages[index];
            }
            return null;
        default:
            return null;
    }
};

watchEffect(() => {
    if (props.id) {
        const id = parseInt(props.id, 10);
        const om_ref = useOpModeStore().get_op_mode(id);
        if (om_ref) {
            om.value = op_mode.fromObject(om_ref).asObject(); // Deep copy
        } else {
            console.error('Op mode not found for ID:', id);
        }
    } else {
        om.value = new op_mode().asObject();
    }
});

const at_max_freq_ranges = computed(() => {
    return !(om.value.freq_ranges.length >= config['MAX_FREQ_RANGES']);
});

function fr_state(index, type) {
    const start_freq = conv_freq(om.value.freq_ranges[index].start);
    const end_freq = conv_freq(om.value.freq_ranges[index].end);
    if (type === 0 && !start_freq) {
        return false;
    }
    if (type === 1 && !end_freq) {
        return false;
    }
    return true;
}

const freq_inputs_error = ref(null);
function addFrequencyRange() {
    const fr = new freq_range(freq_ranges_start.value, freq_ranges_end.value);
    freq_inputs_error.value = fr.validate();

    if (!freq_inputs_error.value) {
        om.value.freq_ranges.push(fr.asObject());
        freq_ranges_start.value = 0;
        freq_ranges_end.value = 0;
    }
}
function removeFrequencyRange(index) {
    if (index >= 0 && index < om.value.freq_ranges.length) {
        om.value.freq_ranges.splice(index, 1);
    }
}

function parse_json() {
    try {
        const parsed = JSON.parse(op_mode_text.value);
        if (parsed && typeof parsed === 'object') {
            om.value = op_mode.fromObject(parsed).asObject();
            validate();
        } else {
            console.error('Parsed JSON is not a valid object:', parsed);
        }
    } catch (e) {
        console.error('Error parsing JSON:', e);
    }
}
function save_op_mode() {
    if (Object.keys(error_messages).length > 0) {
        console.log('Trying to save with Validation errors:', error_messages);
        return;
    }

    const o = op_mode.fromObject(om.value);
    if (props.edit === 'true') {
        console.log('Updating op mode:', o.name);
        o.update();
    } else {
        console.log('Creating op mode:', om.value.name);
        o.create();
    }
    router.back();
}

function cancel() {
    router.back();
}
</script>