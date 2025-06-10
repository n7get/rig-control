<template>
    <div>
        <div class="m-3 pb-2 d-flex justify-content-between border-bottom">
            <div @click="cancel"><u>Cancel</u></div>
            <div><b>EDIT MEMORY CHANNEL</b></div>
            <div @click="save_mem_chan"><u>Save</u></div>
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
                    v-model.lazy="mc.name"
                    :state="validate('name')"
                    required
                ></b-form-input>
                <div v-if="errors('name')" class="text-danger">{{ errors('name') }}</div>
            </b-form-group>

            <b-form-group
                class="mt-2"
                label="Frequency:"
                label-for="frequency"
            >
                <b-form-input
                    id="frequency"
                    ref="frequency-input"
                    type="number"
                    v-model.lazy="mc.frequency"
                    :state="validate('frequency')"
                    required
                ></b-form-input>
                <div v-if="errors('frequency')" class="text-danger">{{ errors('frequency') }}</div>
            </b-form-group>

            <b-form-group
                class="mt-2"
                label="Op Mode:"
                label-for="op_mode"
            >
                <b-form-input
                    id="op_mode"
                    ref="op_mode-input"
                    v-model.lazy="mc.op_mode"
                    :state="validate('op_mode')"
                    required
                ></b-form-input>
                <div v-if="errors('op_mode')" class="text-danger">{{ errors('op_mode') }}</div>
            </b-form-group>

            <b-form-group
                class="mt-2"
                label="Commands:"
                label-for="commands"
            >
                <b-form-textarea
                    id="commands"
                    v-model="mc.commands"
                    placeholder="Enter commands..."
                    rows="4"
                    max-rows="8"
                ></b-form-textarea>
            </b-form-group>

            <div v-if="props.edit != 'true'" class="mt-4 border-top pt-4">
                <b-form-textarea
                    id="textarea"
                    v-model="mem_chan_text"
                    placeholder="Paste Memory Channel JSON text here..."
                    rows="6"
                    max-rows="12"
                ></b-form-textarea>
                <b-button class="mt-2" variant="primary" @click="parse_json">Parse JSON</b-button>
            </div>
        </div>
    </div>
</template>

<script setup>
import { computed, watchEffect, ref } from 'vue';
import { useMemChanStore } from '@/stores/mem_chans';
import { mem_chan } from '@/js/mem_chan';
import { useRouter } from 'vue-router';

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

const mc = ref({});
const mem_chan_text = ref('');

let error_messages = {};
function validate(field) {
    const m = mem_chan.fromObject(mc.value);
    error_messages = m.validate(field);
    return !error_messages[field];
}

const errors = (field) => {
    return error_messages[field] || null;
};

watchEffect(() => {
    if (props.id) {
        const id = parseInt(props.id, 10);
        const mc_ref = useMemChanStore().get_mem_chan(id);
        if (mc_ref) {
            mc.value = mem_chan.fromObject(mc_ref).asObject(); // Deep copy
        } else {
            console.error('Memory channel not found for ID:', id);
        }
    } else {
        mc.value = new mem_chan().asObject();
    }
});

function parse_json() {
    try {
        const parsed = JSON.parse(mem_chan_text.value);
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
function save_mem_chan() {
    if (Object.keys(error_messages).length > 0) {
        console.log('Trying to save with Validation errors:', error_messages);
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
</script>
