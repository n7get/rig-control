<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Memory Channels</b></div>
        <div @click="add_mem_chan"><u>Add</u></div>
    </div>
    <div class="my-3">
        <b-list-group>
            <b-list-group-item class="px-2 d-flex justify-content-between align-items-left"
                v-for="mc in mem_chans"
                :key="mc.id"
            >
                <div class="d-flex justify-content-between w-100">
                    <div @click="select_mem_chan(mc.id)">{{ mc.name }}</div>
                    <div class="d-flex gap-3">
                        <IBiPencil @click="edit_mem_chan(mc.id)" />
                        <IBiCopy @click="copy_mem_chan(mc.id)" />
                        <IBiTrash3 @click="remove_mem_chan(mc.id)" />
                    </div>
                </div>
            </b-list-group-item>
        </b-list-group>
    </div>
    
    <b-modal
        @ok="remove_confirmed"
        @esc="close_confirm_modal"
        @cancel="close_confirm_modal"
        v-model="open_confirm_modal"
        title="Confirm Remove"
        ok-title="Remove"
    >
        <div flush>
            <div>
                Are you sure you want to remove {{ mem_chan_name }}?
            </div>
        </div>
    </b-modal>
</template>

<script setup>
import { computed, ref } from 'vue';
import { mem_chan } from '@/js/mem_chan';
import { useMemChanStore } from '@/stores/mem_chans';
import IBiPencil from '~icons/bi/pencil';
import IBiCopy from '~icons/bi/copy';
import IBiTrash3 from '~icons/bi/trash3';
import { useRouter } from 'vue-router';

const router = useRouter();

function back() {
    router.back();
}

const open_confirm_modal = ref(false);
let mem_chan_id = -1;
const mem_chan_name = ref('');

const mem_chans = computed(() => {
    return Object.values(useMemChanStore().mem_chans).sort((a, b) => a.name.localeCompare(b.name));
});

function select_mem_chan(id) {
    const mc = mem_chan.fromObject(useMemChanStore().get_mem_chan(id));
    if (!mc) {
        console.error('Mem chan not found for: ', id);
        return;
    }

    mc.set_current();

    router.back();
}

function remove_mem_chan(id) {
    mem_chan_id = id;
    const mc_ref = useMemChanStore().get_mem_chan(id);
    mem_chan_name.value = mc_ref.name;
    open_confirm_modal.value = true;
}
function remove_confirmed() {
    if (mem_chan_id === -1) {
        console.error('No mem chan selected for removal');
        return;
    }
    const mc = mem_chan.fromObject(useMemChanStore().get_mem_chan(mem_chan_id));
    if (!mc) {
        console.error('Mem chan not found for: ', mem_chan_id);
        return;
    }
    mc.remove();
    mem_chan_id = -1;
    mem_chan_name.value = '';
    close_confirm_modal();
}
function close_confirm_modal() {
    open_confirm_modal.value = false;
}
function add_mem_chan() {
    router.push({ name: 'mem_chan_edit', params: { edit: false } });
}
function copy_mem_chan(id) {
    router.push({ name: 'mem_chan_edit', params: { id: id, edit: false } });
}
function edit_mem_chan(id) {
    router.push({ name: 'mem_chan_edit', params: { id: id, edit: true } });
}
</script>
