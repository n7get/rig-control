<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Import Groups</b></div>
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
            <b-button @click="import_group">Import</b-button>
        </div>
        <div id="error-messages" class="text-danger mt-2">
            <p v-for="(error, index) in error_messages" :key="index">{{ error }}</p>
        </div>
    </div>
</template>

<script lang="ts" setup>
import { ref } from 'vue';
import { group } from '@/js/group.js';
import { useGroupsStore } from '@/stores/groups';
import { useRouter } from 'vue-router';

const router = useRouter();
const jsonText = ref('');
const error_messages = ref([]);
const { groups } = useGroupsStore();

function do_import(json) {
    const g = group.fromObject(json);
    const result = Object.values(g.validate());
    result.forEach((error) => {
        if (error) {
            error_messages.value.push(error);
        }
    });
    if (result.length > 0) {
        return;
    }
    const existing = Object.values(groups).find((m) => m.name === g.name);
    if (existing) {
        error_messages.value.push(`Group with name "${g.name}" already exists.`);
        return;
    }
    g.create();
}
function import_group() {
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
        error_messages.value.push('Failed to import groups: ' + error);
    }
    error_messages.value.push('Groups import completed.');
}
function back() {
    router.back();
}
const open = (view) => {
    router.push({ name: view });
};
</script>
