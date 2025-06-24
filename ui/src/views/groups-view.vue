<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Groups</b></div>
        <div @click="add_group"><u>Add</u></div>
    </div>
    <div class="my-3">
        <b-list-group>
            <b-list-group-item class="px-2 d-flex justify-content-between align-items-left"
                v-for="grp in groups"
                :key="grp.id"
            >
                <div class="d-flex justify-content-between w-100">
                    <div @click="select_group(grp.id)">{{ grp.name }}</div>
                    <div class="d-flex gap-3">
                        <icon-edit @click="edit_group(grp.id)" />
                        <icon-copy @click="copy_group(grp.id)" />
                        <icon-trash @click="remove_group(grp.id)" />
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
                Are you sure you want to remove {{ group_name }}?
            </div>
        </div>
    </b-modal>
</template>

<script setup>
import { computed, ref } from 'vue';
import { group } from '@/js/group';
import { useGroupsStore } from '@/stores/groups';
import iconEdit from '@/components/icons/edit.vue';
import iconCopy from '@/components/icons/copy.vue';
import iconTrash from '@/components/icons/trash.vue';
import { useRouter } from 'vue-router';

const router = useRouter();

function back() {
    router.back();
}

const open_confirm_modal = ref(false);
let group_id = -1;
const group_name = ref('');

const groups = computed(() => {
    return Object.values(useGroupsStore().groups).sort((a, b) => a.name.localeCompare(b.name));
});

function select_group(id) {
    const grp = group.fromObject(useGroupsStore().get_group(id));
    if (!grp) {
        console.error('Group not found for: ', id);
        return;
    }
    // Implement set_current or similar if needed
    router.back();
}

function remove_group(id) {
    group_id = id;
    const grp_ref = useGroupsStore().get_group(id);
    group_name.value = grp_ref.name;
    open_confirm_modal.value = true;
}
function remove_confirmed() {
    if (group_id === -1) {
        console.error('No group selected for removal');
        return;
    }
    const grp = group.fromObject(useGroupsStore().get_group(group_id));
    if (!grp) {
        console.error('Group not found for: ', group_id);
        return;
    }
    grp.remove();
    group_id = -1;
    group_name.value = '';
    close_confirm_modal();
}
function close_confirm_modal() {
    open_confirm_modal.value = false;
}
function add_group() {
    router.push({ name: 'group_edit', params: { edit: false } });
}
function copy_group(id) {
    router.push({ name: 'group_edit', params: { id: id, edit: false } });
}
function edit_group(id) {
    router.push({ name: 'group_edit', params: { id: id, edit: true } });
}
</script>
