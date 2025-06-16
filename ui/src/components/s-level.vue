<template>
    <b-row>
        <b-col cols="3" class="text-nowrap">
            {{ label }}<span v-if="show !== 'none'"> ({{ event_value }})</span>
        </b-col>
        <b-col class="ps-0">
            <b-form-input
                v-model="event_value"
                type="range"
                :min="min"
                :max="max"
                @change="sendChange"
            ></b-form-input>
        </b-col>
    </b-row>
</template>

<script setup>
import { computed, ref } from 'vue';
import { rig_property } from '@/js/rig_property.js';

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
});

const event_prop = rig_property(props.event);
const event_value = ref(parseInt(event_prop.value, 10) || 0);

const sendChange = () => {
    event_prop.update(event_value.value);
};
</script>
