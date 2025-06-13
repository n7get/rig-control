<template>
    <div>
        <div class="m-3 pb-2 d-flex justify-content-between border-bottom">
            <div @click="back"><u>Back</u></div>
            <div><b>Inmport Op Modes</b></div>
            <div></div>
        </div>
    </div>
    <div class="container">
        <div>
            <b-form-textarea
                id="json-text"
                v-model="jsonText"
                rows="6"
                ></b-form-textarea>
        </div>
        <div class="mt-2">
            <b-button @click="import_op_mode">Import</b-button>
        </div>
        <div id="error-messages" class="text-danger mt-2">
            <p v-for="(error, index) in error_messages" :key="index">{{ error }}</p>
        </div>
    </div>
</template>

<script lang="ts" setup>
import { ref, onBeforeMount } from 'vue';
import { op_mode } from '@/js/op_mode.js';
import { useOpModeStore } from '@/stores/op_modes';
import { useRouter } from 'vue-router';

const router = useRouter();

const jsonText = ref('');
const error_messages = ref([]);
const { op_modes } = useOpModeStore();

function do_import(json) {
    const mc = op_mode.fromObject(json);

    const result = Object.values(mc.validate());
    result.forEach((error) => {
        if (error) {
            error_messages.value.push(error);
        }
    });
    if (result.length > 0) {
        return;
    }

    const existing = Object.values(op_modes).find((m) => m.name === mc.name);
    if (existing) {
        error_messages.value.push(`Op Mode with name "${mc.name}" already exists.`);
        return;
    }

    mc.create();
}
function import_op_mode() {
    error_messages.value = [];
    try {
        const parsedData = JSON.parse(jsonText.value);
        if (Array.isArray(parsedData)) {
            parsedData.forEach((parsed) => {
                do_import(parsed);
            });
        } else {
            do_import(parsedData);
        }
    } catch (error) {
        error_messages.value.push('Failed to import Op Modes:', error);
    }
    error_messages.value.push('Op Mode import completed.');
}

function back() {
    router.back();
}

const open = (view) => {
    router.push({ name: view });
};
</script>
