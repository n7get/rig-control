<template>
    <div class="d-flex justify-content-center py-2">
        <b-button
            class="mb-2"
            variant="primary"
            @click="open_modal('repeater')"
            v-show="mode.value === 'FM'"
        >
            Repeater Settings
        </b-button>
    </div>
</template>

<script setup>
import { rig_property } from '@/js/rig_property.js';
import { useGlobalStore } from '@/stores/global';

const mode = rig_property('mode');

const open_modal = (name) => {
    useGlobalStore().openModal(name);
}
</script>
