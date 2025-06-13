<template>
    <div>
        <div class="m-3 pb-2 d-flex justify-content-between border-bottom">
            <div @click="back"><u>Back</u></div>
            <div><b>Export Memory Channels</b></div>
            <div></div>
        </div>
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
import { useMemChanStore } from '@/stores/mem_chans';
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
    const { mem_chans } = useMemChanStore();
    const mc_list = Object.values(mem_chans).map((mc) => {
        return {
            name: mc.name,
            frequency: mc.frequency,
            op_mode: mc.op_mode,
            commands: mc.commands,
        };
    });
    jsonText.value = JSON.stringify(mc_list, null, 2);
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