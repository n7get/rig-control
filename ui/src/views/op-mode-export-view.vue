<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Export Op Modes</b></div>
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
            <!-- <b-button @click.prevent="download">Download</b-button> -->
            <b-button @click="copyToClipboard">Copy to Clipboard</b-button>
        </div>
    </div>
</template>

<script lang="ts" setup>
import { ref, onBeforeMount } from 'vue';
import { useOpModeStore } from '@/stores/op_modes';
import { useRouter } from 'vue-router';

const router = useRouter();

const props = defineProps({
    edit: {
        type: Boolean,
        default: false
    }
});

const jsonText = ref('');

onBeforeMount(() => {
    const { op_modes } = useOpModeStore();
    const om_list = Object.values(op_modes)
        .filter((om) => {
            return om.name !== 'Default';
        })
        .map((om) => {
            return {
                name: om.name,
                freq_ranges: om.freq_ranges,
                order: om.order,
                commands: om.commands,
            };
        });
    jsonText.value = JSON.stringify(om_list, null, 2);
});

// function download() {
//     var link = document.createElement('a');

//     link.href = '/noderig/' + this.topic + '.json';
//     link.download = this.topic + '.json';
//     link.click();

//     this.closeExportJson();
// }
function copyToClipboard() {
    var copyText = document.getElementById("json-text");
    copyText.select(); 
    copyText.setSelectionRange(0, 99999);
    document.execCommand("copy");

    this.closeExportJson();
}

function back() {
    router.back();
}

const open = (view) => {
    router.push({ name: view });
};
</script>
