<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Inmport Op Modes</b></div>
        <div></div>
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

    const validationErrors = mc.validate(); 
    if (Object.keys(validationErrors).length > 0) {
        error_messages.value.push(`Op Mode ${mc.name} import failed:`);

        Object.entries(validationErrors).forEach(([field, errors]) => {
            if (field === 'freq_ranges') {
                for (const [index, fr] of json.freq_ranges.entries()) {
                    const msg = errors[index];
                    if (msg) {
                        error_messages.value.push(`Freq Range ${fr.start}, ${fr.end}: ${msg}`);
                    }
                }
            }
            else {
                error_messages.value.push(errors);
            }
        });

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
