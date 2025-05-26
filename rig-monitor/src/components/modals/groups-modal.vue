<template>
    <b-modal
        @ok="close_modal"
        @esc="close_modal"
        v-model="open_modal"
        title="Select Group"
        ok-only
        ok-title="Close"
    >
        <b-list-group>
            <b-list-group-item v-for="group in list" :key="group" class="pl-5">
                <div @click="select_group(group)">{{ group }}</div>
            </b-list-group-item>
        </b-list-group>
    </b-modal>
</template>

<script setup>
import { computed, useTemplateRef, ref } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { useGroupsStore } from '@/stores/groups';

const open_modal = ref(true);

const list = computed(() => {
    const groups_list = useGroupsStore().groups_list;
    return Object.keys(groups_list);
});

const select_group = (group) => {
    const groups_list = useGroupsStore().groups_list;
    const g = groups_list[group];
    useGroupsStore().set_current_group(g);
    close_modal();
};

function close_modal() {
    useGlobalStore().closeModal();
}

</script>
