<template>
<b-row>
    <div class="col-4">
        {{ meter }}
    </div>
    <div class="alc-meter col mt-1">
        <b-progress :value="value" max="255"></b-progress>
    </div>
</b-row>
</template>

<script setup>
import { computed } from 'vue';
import { rig_property } from '@/js/rig_property.js';

const props = defineProps({
    meter: {
        type: String,
        default: 'meter',
        validator: function (value) {
            return ['COMP', 'ALC', 'PO', 'SWR', 'ID', 'VDD'].indexOf(value) !== -1
        },
    }
});
const read_meter_front_panel = rig_property('read_meter_front_panel');

const value = computed(() => {
    return read_meter_front_panel.value + '%';
});
</script>

<style scoped>
.alc-meter {
    padding-left:0;
}
</style>
