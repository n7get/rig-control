<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="cancel"><u>Cancel</u></div>
        <div><b>EDIT OP MODE</b></div>
        <div @click="save_op_mode"><u>Save</u></div>
    </div>
    <div class="my-3">
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
                    <b-button variant="danger" @click="remove_frequency_range(index)"><IBiXLg /></b-button>
                </div>
                <div v-if="errors('freq_ranges', index)" class="text-danger"><i>{{ errors('freq_ranges', index) }}</i></div>
            </div>

            <div>
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
                    <b-button variant="primary" @click="add_frequency_range()"><IBiPlus /></b-button>
                </div>
                <div v-if="freq_inputs_error" class="text-danger"><i>{{ freq_inputs_error }}</i></div>
            </div>
        </b-form-group>

        <command-list
            @add-command="add_command"
            @remove-command="remove_command"
            @update-command="update_command"
            :commands="om.commands" />
    </div>
</template>

<script setup>
import { computed, onBeforeMount, ref } from 'vue';
import { useOpModeStore } from '@/stores/op_modes';
import IBiPlus from '~icons/bi/plus';
import IBiXLg from '~icons/bi/x-lg';
import { useRouter } from 'vue-router';
import { rig_property } from '@/js/rig_property.js';
import { config, freq_range, op_mode } from '@/js/op_mode';
import { conv_freq, is_in_band } from '@/js/freq_utils';
import commandList from '@/components/command-list.vue';

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
        default:
            return null;
    }
};

onBeforeMount(() => {
    if (props.id) {
        const om_ref = useOpModeStore().get_op_mode(props.id);
        if (om_ref) {
            om.value = op_mode.fromObject(om_ref).asObject(); // Deep copy
            if (props.edit === 'false') {
                om.value.id = 0;
            }
        } else {
            console.error('Op mode not found for ID:', props.id);
        }
    } else {
        om.value = new op_mode().asObject();
        const rp = rig_property('mode');
        add_command({ name: rp.name, value: ref(rp.value) });
    }
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
function add_frequency_range() {
    const fr = new freq_range(freq_ranges_start.value, freq_ranges_end.value);
    freq_inputs_error.value = fr.validate();

    if (!freq_inputs_error.value) {
        om.value.freq_ranges.push(fr.asObject());
        freq_ranges_start.value = 0;
        freq_ranges_end.value = 0;
    }
}
function remove_frequency_range(index) {
    if (index >= 0 && index < om.value.freq_ranges.length) {
        om.value.freq_ranges.splice(index, 1);
    }
}

function add_command(command) {
    const isPresent = om.value.commands.find(c => c.name === command.name);
    if (isPresent) {
        console.warn('Command already exists:', command.name);
        return;
    }
    om.value.commands.push(command);
}
function remove_command(name) {
    const index = om.value.commands.findIndex(c => c.name === name);
    if (index !== -1) {
        om.value.commands.splice(index, 1);
    }
}
function update_command(command) {
    const index = om.value.commands.findIndex(c => c.name === command.name);
    if (index !== -1) {
        om.value.commands[index].value = command.value;
    } else {
        console.warn('Command not found for update:', command.name);
    }
}

function save_op_mode() {
    if (Object.keys(error_messages).length > 0) {
        return;
    }

    const o = op_mode.fromObject(om.value);
    if (props.edit === 'true') {
        o.update();
    } else {
        o.create();
    }
    router.back();
}

function cancel() {
    router.back();
}
</script>
