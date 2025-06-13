<template>
    <div>
        <div class="m-3 pb-2 d-flex justify-content-between border-bottom">
            <div @click="cancel"><u>Cancel</u></div>
            <div><b>EDIT MEMORY CHANNEL</b></div>
            <div @click="save_mem_chan"><u>Save</u></div>
        </div>
        <div class="m-3">
            <b-form-group class="mt-2" label="Name:" label-for="name">
                <b-form-input
                    id="name"
                    v-model.lazy="mc.name"
                    :state="validate('name')"
                    required
                ></b-form-input>
                <div v-if="errors('name')" class="text-danger">{{ errors('name') }}</div>
            </b-form-group>

            <b-form-group class="mt-2" label="Frequency:" label-for="frequency">
                <b-form-input
                    id="frequency"
                    type="number"
                    v-model.lazy="mc.frequency"
                    :state="validate('frequency')"
                    @blur="check_freq"
                    required
                ></b-form-input>
                <div v-if="errors('frequency')" class="text-danger">{{ errors('frequency') }}</div>
            </b-form-group>

            <b-form-group class="mt-2" label="Op Mode:" label-for="op_mode">
                <b-form-select
                    id="op_mode"
                    v-model="mc.op_mode"
                    :options="op_mode_options"
                    :state="validate('op_mode')"
                    required
                ></b-form-select>
                <div v-if="errors('op_mode')" class="text-danger">{{ errors('op_mode') }}</div>
            </b-form-group>

            <command-list 
                @add-command="add_command"
                @remove-command="remove_command"
                @update-command="update_command"
                :commands="mc.commands" />

            <div v-if="props.edit != 'true'" class="mt-4 border-top pt-4">
                <b-form-group
                    class="mt-2"
                    label="Mem Chan JSON:"
                    label-for="mem-chan-json"
                >
                    <b-form-textarea
                        id="mem-chan-json"
                        v-model="mem_chan_json"
                        placeholder="Pase Mem Chan JSON text here..."
                        rows="6"
                        max-rows="12" />
                    <b-button class="mt-2" variant="primary" @click="parse_json">Parse JSON</b-button>
                </b-form-group>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, onBeforeMount, computed } from 'vue';
import { useMemChanStore } from '@/stores/mem_chans';
import { useOpModeStore } from '@/stores/op_modes';
import { mem_chan } from '@/js/mem_chan';
import { conv_freq, format_freq } from '@/js/freq_utils.js';
import { useRouter } from 'vue-router';

const op_mode_options = computed(() => {
    const modes = Object.values(useOpModeStore().op_modes).sort((a, b) => {
        if (a.name === 'Default') return 1;
        if (b.name === 'Default') return -1;
        return a.name.localeCompare(b.name);
    }).map(om => ({value: om.name, text: om.name}));

    return [{ value: '', text: '' }, ...modes];
});

const router = useRouter();
const props = defineProps({
    id: { type: String },
    edit: { type: String, required: true },
});

const mc = ref({});
let error_messages = {};

function check_freq() {
    var new_freq = conv_freq(mc.value.frequency);

    if(new_freq) {
        mc.value.frequency = new_freq;
    }
    else {
        freq_input_state.value = false;
    }
}

function validate(field) {
    const m = mem_chan.fromObject(mc.value);
    error_messages = m.validate(field);
    return Object.keys(error_messages).length === 0;
}

const errors = (field) => error_messages[field] || null;

onBeforeMount(() => {
    if (props.id) {
        const mc_ref = useMemChanStore().get_mem_chan(props.id);
        if (mc_ref) {
            mc.value = mem_chan.fromObject(mc_ref).asObject();
        } else {
            console.error('Mem chan not found for ID:', props.id);
        }
    } else {
        mc.value = new mem_chan().asObject();
    }
});

function add_command(command) {
    const isPresent = mc.value.commands.find(c => c.name === command.name);
    if (isPresent) {
        console.warn('Command already exists:', command.name);
        return;
    }
    mc.value.commands.push(command);
}
function remove_command(name) {
    const index = mc.value.commands.findIndex(c => c.name === name);
    if (index !== -1) {
        mc.value.commands.splice(index, 1);
    }
}
function update_command(command) {
    const index = mc.value.commands.findIndex(c => c.name === command.name);
    if (index !== -1) {
        mc.value.commands[index].value = command.value;
    } else {
        console.warn('Command not found for update:', command.name);
    }
}

function save_mem_chan() {
    if (Object.keys(error_messages).length > 0) {
        return;
    }
    const m = mem_chan.fromObject(mc.value);
    if (props.edit === 'true') {
        m.update();
    } else {
        m.create();
    }
    router.back();
}

function cancel() {
    router.back();
}

const mem_chan_json = ref('');

function parse_json() {
    try {
        const parsed = JSON.parse(mem_chan_json.value);
        if (parsed && typeof parsed === 'object') {
            mc.value = mem_chan.fromObject(parsed).asObject();
            validate();
        } else {
            console.error('Parsed JSON is not a valid object:', parsed);
        }
    } catch (e) {
        console.error('Error parsing JSON:', e);
    }
}
</script>
