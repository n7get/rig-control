<template v-if="modal.open">
    <number-modal :min="min" :max="max" :step="step" :fast_step="fast_step" :name="name" v-if="open_modal === 'number'" />
    <list-modal :name="name" :list="list" v-if="open_modal === 'list'" />

    <vfo-modal vfo="vfo_a" v-if="modal.name === 'vfo_a'" />
    <vfo-modal vfo="vfo_b" v-if="modal.name === 'vfo_b'" />
</template>

<script setup>
import { ref } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { rig_property } from '@/js/rig_property.js';

import booleanModal from './boolean_modal.vue';
import listModal from './list_modal.vue';
import numberModal from './number_modal.vue';
import vfoModal from './vfo_modal.vue';
import { computed } from 'vue';

const modal = useGlobalStore().modal;

const name = ref('name');
const min = ref(0);
const max = ref(100);
const step = ref(1);
const fast_step = ref(10);
const list = ref([]);

const open_modal = computed(() => {
    if (modal.open) {
        const rp = rig_property(modal.name);

        if (rp.list) {
            name.value = rp.name;
            list.value = rp.list;
            return 'list';
        }

        const range = rp.range;
        if (range) {
            min.value = range.hasOwnProperty('min') ? range.min : '0';
            max.value = range.hasOwnProperty('max') ? range.max : '100';
            step.value = range.hasOwnProperty('step') ? range.step : '1';
            fast_step.value = range.hasOwnProperty('fast_step') ? range.fast_step : '10';
            name.value = rp.name;
            return 'number';
        }
        return undefined;
    }

});
</script>
