<template>
    <b-card class="pb-2" no-body border-variant="secondary">
        <b-card-header class="d-flex justify-content-center" @click="$emit('header-click')">
            <div text-variant="white" bg-variant="secondary">
                 {{ title }}
            </div>
        </b-card-header>
        <div v-if="open" flush>
            <div class="px-2 d-flex justify-content-between align-items-left bg-secondary text-light">
                <div>Description</div>
                <div>Value</div>
            </div>
            <b-list-group>
                <b-list-group-item class="px-2 d-flex justify-content-between align-items-left"
                    v-for="rp in prop_list"
                    :key="rp.name"
                    @click="$emit('property-click', rp)">

                    <div :class="{ unavailable: rp.unavailable }">{{ rp.desc }}<span v-if="rp.changed">*</span></div>
                    <div :class="{ unavailable: rp.unavailable }">{{ rp.value }}{{ rp.suffix }}</div>
                </b-list-group-item>
            </b-list-group>
        </div>
    </b-card>
</template>

<script setup>
import { computed, ref } from 'vue';
import { get_property_list } from '@/js/rig_property.js';

const props = defineProps({
    list: {
        type: Object,
        required: true,
    },
    open: {
        type: Boolean,
        default: true,
    },
    title: {
        type: String,
        default: 'Settings',
    },
});

const prop_list = computed(() => get_property_list(props.list));
</script>