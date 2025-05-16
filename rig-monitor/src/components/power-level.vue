<template>
    <b-row>
        <div @click="toggle" class="col-4 mt-1">Power({{ power_level.value }})</div>
        <div v-if="pl_buttons" class="col-7 px-0">
            <button @click="powerLevel(5)" class="pl-btn" :class="{selected: power_level.value === 5}">5</button>
            <button @click="powerLevel(10)" class="pl-btn" :class="{selected: power_level.value === 10}">10</button>
            <button @click="powerLevel(20)" class="pl-btn" :class="{selected: power_level.value === 20}">20</button>
            <button @click="powerLevel(50)" class="pl-btn" :class="{selected: power_level.value === 50}">50</button>
            <button @click="powerLevel(100)" :class="{selected: power_level.value === 100}">100</button>
        </div>
        <b-form-input
            v-if="!pl_buttons"
	        class="col-7"
            v-model="power_level.value"
            type="range"
            min="0"
            max="100"
            @change="sendChange"
        ></b-form-input>
    </b-row>
</template>

<script setup>
import { computed, ref } from 'vue';
import { send_command } from '@/js/web_socket.js';
import { useSettingsStore } from '@/stores/settings';

const pl_buttons = ref(true);
const power_level = useSettingsStore().power_level;

const toggle = () => {
    pl_buttons.value = !pl_buttons.value;
};
const powerLevel = (new_value) => {
    send_command('power_level', new_value);
};
const sendChange = () => {
    if (power_level.value < 5) {
        power_level.value = 5;
    }
    send_command('power_level', power_level.value);
};
</script>

<style scoped>
    .pl-btn {
        display: inline-block;
        min-width: 45px;
    }
    .selected {
        background-color: #bbb;
    }
</style>
