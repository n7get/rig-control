<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Groups</b></div>
        <div></div>
    </div>
    <div class="container">
        <b-button class="mb-3 w-100" @click="open('groups')">Group List</b-button>
        <b-button class="mb-3 w-100" @click="open('group_import')">Import Groups</b-button>
        <b-button class="mb-3 w-100" @click="open('group_export')">Export Groups</b-button>
        <b-button class="mb-3 w-100 btn-danger" @click="group_reset">Groups Reset</b-button>
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
                You are about to remove all Groups. Are you sure you want to continue?
            </div>
        </div>
    </b-modal>
</template>

<script lang="ts" setup>
import { ref } from 'vue';
import { useRouter } from 'vue-router';
import { send_message } from '@/js/web_socket.js';

const router = useRouter();
const open_confirm_modal = ref(false);

function group_reset() {
    open_confirm_modal.value = true;
}
function reset_confirmed() {
    close_confirm_modal();
    send_message({'topic': 'group', 'event': 'reset'});
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
