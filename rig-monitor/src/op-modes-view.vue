<template>
    <div>
        <div class="mx-3 my-2 pb-2 d-flex justify-content-between border-bottom">
            <div @click="back"><u>Back</u></div>
            <div><b>OP MODES</b></div>
            <div @click="add_op_mode"><u>Add</u></div>
        </div>
        <div class="m-3">
            <div>
                <b-list-group>
                    <b-list-group-item class="px-2 d-flex justify-content-between align-items-left"
                        v-for="om in op_modes"
                        :key="om.name"
                    >
                        <div class="d-flex justify-content-between w-100">
                            <div @click="select_op_mode(om.id)">{{ om.name }}</div>
                            <div class="d-flex gap-3">
                                <IBiPencil @click="edit_op_mode(om.id)" />
                                <IBiTrash3 @click="remove_op_mode(om.id)" />
                            </div>
                        </div>
                    </b-list-group-item>
                </b-list-group>
            </div>
        </div>
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
                Are you sure you want to remove {{ op_mode_name }}?
            </div>
        </div>
    </b-modal>

    <op-mode-edit v-if="open_edit_modal" :name="edit_name" :edit="true" />
</template>

<script setup>
import { computed, onMounted, ref } from 'vue';
import { op_mode } from '@/js/op_mode';
import { useOpModeStore } from '@/stores/op_modes';
import { send_message } from '@/js/web_socket';
import IBiPencil from '~icons/bi/pencil';
import IBiTrash3 from '~icons/bi/trash3';
import IBiPlus from '~icons/bi/plus';
import opModeEdit from '@/components/op-mode-edit.vue';
import { useRouter } from 'vue-router';

const router = useRouter();

function back() {
    router.back();
}

const op_modes = useOpModeStore().op_modes
const open_list_modal = ref(false);
const open_confirm_modal = ref(false);
const edit_name = ref('');
const open_edit_modal = ref(false);

let op_mode_id = -1;
const op_mode_name = ref('');

function select_op_mode(id) {
    const om = op_mode.fromObject(useOpModeStore().get_op_mode(id));
    if (!om) {
        console.error('Op mode not found for: ', id);
        return;
    }

    om.set_current();

    router.back();
}

function remove_op_mode(id) {
    op_mode_id = id;
    const om_ref = useOpModeStore().get_op_mode(id);
    op_mode_name.value = om_ref.name;
    open_confirm_modal.value = true;
}
function remove_confirmed() {
    if (op_mode_id === -1) {
        console.error('No op mode selected for removal');
        return;
    }

    const om = op_mode.fromObject(useOpModeStore().get_op_mode(op_mode_id));
    if (!om) {
        console.error('Op mode not found for: ', op_mode_id);
        return;
    }

    om.remove();

    op_mode_id = -1;
    op_mode_name.value = '';
    close_confirm_modal();
}

function close_confirm_modal() {
    open_confirm_modal.value = false;
}

function edit_op_mode(id) {
    edit_name.value = id;
    open_edit_modal.value = true;
}
</script>
