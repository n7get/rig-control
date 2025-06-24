<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Export Groups</b></div>
        <div></div>
    </div>
    <div class="container">
        <div>
            <b-form-textarea
                id="json-text"
                v-model="jsonText"
                readonly
                rows="6"
            ></b-form-textarea>
        </div>
        <div class="mt-2">
            <b-button @click="copyToClipboard">Copy to Clipboard</b-button>
        </div>
    </div>
</template>

<script lang="ts" setup>
import { ref, onBeforeMount } from 'vue';
import { useGroupsStore } from '@/stores/groups';
import { useRouter } from 'vue-router';

const router = useRouter();
const jsonText = ref('');

onBeforeMount(() => {
    const { groups } = useGroupsStore();
    const group_list = Object.values(groups).map((g) => {
        return {
            name: g.name,
            modes: g.modes,
            order: g.order,
            properties: g.properties,
        };
    });
    jsonText.value = JSON.stringify(group_list, null, 2);
});

function copyToClipboard() {
    var copyText = document.getElementById("json-text");
    copyText.select();
    copyText.setSelectionRange(0, 99999);
    document.execCommand("copy");
}
function back() {
    router.back();
}
const open = (view) => {
    router.push({ name: view });
};
</script>
