<template v-if="modal.open">
    <boolean-modal v-if="modal_type === 'boolean'" />
    <number-modal v-if="modal_type === 'number'" />
    <list-modal v-if="modal_type === 'list'" />
    <vfo-modal vfo="vfo_a" v-if="modal_type === 'vfo_a'" />
    <vfo-modal vfo="vfo_b" v-if="modal_type === 'vfo_b'" />
</template>

<script setup>
import { computed } from 'vue';
import { useGlobalStore } from '@/stores/global';

const modal = useGlobalStore().modal;

const modal_type = computed(() => {
    if (modal.open) {
        const rp = modal.rig_prop;

        if (rp.isBoolean()) return 'boolean';
        if (rp.list) return 'list';
        if (rp.range) return 'number';
        if (rp) return rp.name;

        return undefined;
    }
});
</script>
