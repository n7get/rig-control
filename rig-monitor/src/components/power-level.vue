<template>
    <b-row>
        <div @click="toggle" class="col-4 mt-1">Power({{ power_level.value }})</div>
        <div class="col-7 px-0">
            <div v-if="pl_buttons">
                <button @click="powerLevel(5)" class="pl-btn" :class="{selected: power_level.value === 5}">5</button>
                <button @click="powerLevel(10)" class="pl-btn" :class="{selected: power_level.value === 10}">10</button>
                <button @click="powerLevel(20)" class="pl-btn" :class="{selected: power_level.value === 20}">20</button>
                <button @click="powerLevel(50)" class="pl-btn" :class="{selected: power_level.value === 50}">50</button>
                <button @click="powerLevel(100)" :class="{selected: power_level.value === 100}">100</button>
            </div>
            <b-form-input
                v-if="!pl_buttons"
                class="col-7"
                v-model="form_input"
                type="range"
                min="5"
                max="100"
                @change="sendChange"
            />
        </div>
    </b-row>
</template>

<script setup>
import { computed, ref } from 'vue';
import { rig_property } from '@/js/rig_property.js';

const pl_buttons = ref(true);
const power_level = rig_property('power_level');
const form_input = ref(0);

const toggle = () => {
    form_input.value = power_level.value;
    pl_buttons.value = !pl_buttons.value;
};
const powerLevel = (new_value) => {
    power_level.update(new_value);
};
const sendChange = () => {
    if (form_input.value < 5) {
        form_input.value = 5;
    }
    power_level.update(form_input.value);
};
</script>

<style scoped>
    .pl-btn {
        margin-right: 5px;
        display: inline-block;
        min-width: 45px;
    }
    .selected {
        background-color: #bbb;
    }
</style>
