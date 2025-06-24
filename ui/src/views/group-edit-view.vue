<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="cancel"><u>Cancel</u></div>
        <div><b>EDIT GROUP</b></div>
        <div @click="save_group"><u>Save</u></div>
    </div>
    <div class="my-3">
        <b-form-group class="mt-2" label="Name:" label-for="name">
            <b-form-input
                id="name"
                v-model.lazy="grp.name"
                :state="validate('name')"
                required
            ></b-form-input>
            <div v-if="errors('name')" class="text-danger">{{ errors('name') }}</div>
        </b-form-group>
        
        <div class="d-flex justify-content-between align-items-left my-2">
            <div class="my-2">Modes:</div>
            <b-button variant="outline-primary" @click="mode_modal = true">Add</b-button>
        </div>
        <b-list-group>
            <b-list-group-item class="d-flex justify-content-between align-items-left"
                v-for="mode in grp.modes"
                :key="mode"
            >
                <div class="d-flex justify-content-between w-100">
                    <div>{{ mode }}</div>
                    <icon-trash @click="remove_mode(mode)" />
                </div>
            </b-list-group-item>
        </b-list-group>

        <b-form-group class="mt-2" label="Order:" label-for="order">
            <b-form-input
                id="order"
                type="number"
                v-model.lazy="grp.order"
                required
            ></b-form-input>
        </b-form-group>

        <command-list 
            @add-command="add_command"
            @remove-command="remove_command"
            @move-up="move_up"
            @move-down="move_down"
            :properties=true
            :commands="grp.properties" />
    </div>

    <b-modal
        @ok="mode_modal_close"
        @esc="mode_modal_close"
        @close="mode_modal_close"
        v-model="mode_modal"
        title="Add Mode"
        ok-only
        ok-title="Close"
    >
        <b-list-group>
            <b-list-group-item class="d-flex justify-content-between align-items-left"
                v-for="mode in Object.keys(modes_cat)"
                :key="mode"
            >
                <div class="d-flex justify-content-between w-100" @click="add_mode(mode)">
                    <div>{{ mode }}</div>
                    <icon-plus />
                </div>
            </b-list-group-item>
        </b-list-group>
    </b-modal>
</template>

<script setup>
import { ref, onBeforeMount, watch } from 'vue';
import iconTrash from '@/components/icons/trash.vue';
import iconPlus from '@/components/icons/plus.vue';
import { group } from '@/js/group.js';
import { useGroupsStore } from '@/stores/groups';
import { useRouter } from 'vue-router';
import { modes_cat } from '@/js/rig_setting.js';
import { rig_property } from '@/js/rig_property';

const router = useRouter();
const props = defineProps({
    id: { type: String },
    edit: { type: String, required: true },
});

const grp = ref({});
const modesInput = ref('');
let error_messages = {};
const mode_modal = ref(false);

function validate(field) {
    const g = group.fromObject(grp.value);
    error_messages = g.validate(field);
    return Object.keys(error_messages).length === 0;
}
const errors = (field) => error_messages[field] || null;

onBeforeMount(() => {
    if (props.id) {
        const id = parseInt(props.id, 10);
        const g = useGroupsStore().find_group(id);
        grp.value = group.fromObject(g).asObject(); // Deep copy
    } else {
        grp.value = new group().asObject();
    }
    // Initialize modesInput from grp.value.mode (if array or string)
    if (Array.isArray(grp.value.mode)) {
        modesInput.value = grp.value.mode.join(', ');
    } else if (typeof grp.value.mode === 'string') {
        modesInput.value = grp.value.mode;
    }
});

watch(modesInput, (val) => {
    // Update grp.value.mode as an array of trimmed strings
    grp.value.mode = val.split(',').map(m => m.trim()).filter(m => m.length > 0);
});

function save_group() {
    if (Object.keys(error_messages).length > 0) {
        return;
    }
    const g = group.fromObject(grp.value);
    if (props.edit === 'true') {
        g.update();
    } else {
        g.create();
    }
    router.back();
}

function add_mode(mode) {
    if (!grp.value.modes.includes(mode)) {
        grp.value.modes.push(mode);
    }
    mode_modal_close();
}
function remove_mode(name) {
    const index = grp.value.modes.indexOf(name);
    if (index > -1) {
        grp.value.modes.splice(index, 1);
    }
}
function mode_modal_close() {
    mode_modal.value = false;
}

function add_command(command) {
    const isPresent = grp.value.properties.find(name => name === command.name);
    if (isPresent) {
        console.warn('Command already exists:', command.name);
        return;
    }
    grp.value.properties.push(command.name);
}
function remove_command(command_name) {
    const index = grp.value.properties.findIndex(name => name === command_name);
    if (index !== -1) {
        grp.value.properties.splice(index, 1);
    }
}

function move_up(command) {
    const index = grp.value.properties.findIndex(name => name === command.name);
    if (index > 0) {
        const temp = grp.value.properties[index - 1];
        grp.value.properties[index - 1] = grp.value.properties[index];
        grp.value.properties[index] = temp;
    }
}
function move_down(command) {
    const index = grp.value.properties.findIndex(name => name === command.name);
    if (index < grp.value.properties.length - 1) {
        const temp = grp.value.properties[index + 1];
        grp.value.properties[index + 1] = grp.value.properties[index];
        grp.value.properties[index] = temp;
    }
}

function cancel() {
    router.back();
}
</script>
