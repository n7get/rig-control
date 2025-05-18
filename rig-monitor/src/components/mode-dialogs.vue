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
import { useSettingsStore } from '@/stores/settings';
import { useGlobalStore } from '@/stores/global';

const mode = useSettingsStore().mode;
console.log('mode', mode.value);

const open_modal = (name) => {
    useGlobalStore().openModal(name);
}
</script>