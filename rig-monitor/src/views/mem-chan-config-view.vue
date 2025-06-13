<template>
    <div>
        <div class="m-3 pb-2 d-flex justify-content-between border-bottom">
            <div @click="back"><u>Back</u></div>
            <div><b>Memory Channels</b></div>
            <div></div>
        </div>
    </div>
    <div class="container">
        <b-button class="mb-3 w-100" @click="open('mem_chan_import')">Import Memory Channels</b-button>
        <b-button class="mb-3 w-100" @click="open('mem_chan_export')">Export Memory Channels</b-button>
        <b-button class="mb-3 w-100 btn-danger" @click="mem_chan_reset">Memory Channels Reset</b-button>
    </div>

    <b-modal
        @ok="reset_confirmed"
        @esc="close_confirm_modal"
        @cancel="close_confirm_modal"
        v-model="open_confirm_modal"
        title="Confirm Reset"
        ok-title="Reset"
    >
        <div flush>
            <div>
                You are about to remove all Memory Channels. Are you sure you want to continue?
            </div>
        </div>
    </b-modal>
</template>

<script lang="ts" setup>
import { ref } from 'vue';
import { useMemChanStore } from '@/stores/mem_chans';
import { mem_chan } from '@/js/mem_chan.js';
import { useRouter } from 'vue-router';

const router = useRouter();
const open_confirm_modal = ref(false);
const { mem_chans } = useMemChanStore();

function mem_chan_reset() {
    open_confirm_modal.value = true;
}
function reset_confirmed() {
    Object.values(mem_chans)
        .forEach((mc) => {
            const m = mem_chan.fromObject(mc);
            console.log(`Removing Memory Channel: ${m.name}`);
            m.remove();
        });
    close_confirm_modal();
}
function close_confirm_modal() {
    open_confirm_modal.value = false;
}

function back() {
    router.back();
}
const open = (view) => {
    router.push({ name: view });
};
</script>