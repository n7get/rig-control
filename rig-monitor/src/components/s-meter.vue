<template>
    <b-row>
        <b-col cols="3">
            {{ props.label }}
        </b-col>
        <b-col class="mt-1 ps-0">
            <b-progress :value="value" max="255"/>
        </b-col>
    </b-row>
</template>

<script setup>
import { computed } from 'vue';
import { rig_property } from '@/js/rig_property.js';

const props = defineProps({
    meter: {
        type: String,
        required: true,
    },
    label: {
        type: String,
        required: true,
    },
})

const value = computed(() => {
    return rig_property(props.meter).value;
});
</script>
