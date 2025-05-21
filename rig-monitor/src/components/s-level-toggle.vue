<template>
    <b-row>
        <div @click="toggle_prop_update" class="col-4" :class="{disabled: disabled}">
            {{ label }}<span v-if="show !== 'none'"> ({{ event_value }})</span>
        </div>
        <div class="col-7 px-0">
            <b-form-input
                v-model="event_value"
                type="range"
                :min="min"
                :max="max"
                @change="event_prop_update"
                :disabled="disabled"
            ></b-form-input>
        </div>
    </b-row>
</template>

<script setup>
import { computed, ref } from 'vue';
import { rig_property } from '@/js/rig_property.js';
import { send_command } from '@/js/web_socket.js';

const props = defineProps({
    event: {
        type: String,
        required: true,
    },
    label: {
        type: String,
        required: true,
    },
    min: {
        type: Number,
        default: 0,
    },
    max: {
        type: Number,
        max: 100,
    },
    show: {
        type: String,
        default: '',
    },
    'toggle': {
        type: String,
        default: null,
    },
});

const event_prop = rig_property(props.event);
const event_value = ref(0);
event_value.value = parseInt(event_prop.value, 10) || 0;
const toggle_prop = rig_property(props.toggle);

const disabled = computed(() => { return !toggle_prop.value; });
const event_prop_update = () => { event_prop.update(event_value.value); };
const toggle_prop_update = () => { toggle_prop.update(!toggle_prop.value); };
</script>

<style scoped>
    .disabled {
        color: lightgray;
    }
</style>
