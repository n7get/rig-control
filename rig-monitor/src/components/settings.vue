<template>
    <property-list
        :title="title"
        :list="list"
        @header-click="open_groups_modal"
        @property-click="edit_property"
    />

    <b-modal
        @ok="close_modal"
        @esc="close_modal"
        @close="close_modal"
        v-model="groups_modal"
        title="Select Group"
        ok-only
        ok-title="Close"
    >
        <b-list-group>
            <b-list-group-item v-for="group in group_list" :key="group" class="pl-5">
                <div @click="select_group(group)">{{ group }}</div>
            </b-list-group-item>
        </b-list-group>
    </b-modal>
</template>

<script setup>
import { computed, ref } from 'vue';
import { get_property_list } from '@/js/rig_property.js';
import { useGlobalStore } from '@/stores/global';
import { useGroupsStore } from '@/stores/groups';

const props = defineProps({
    list: {
        type: Object,
        required: true,
    },
    title: {
        type: String,
        default: 'Settings',
    },
});

const edit_property = (rp) => {
    if (rp.unavailable) {
        return;
    }

    if (rp.name === 'antenna_tuner_control') {
        rp.update(rp.value === 'on' ? 'off' : 'on');
        return;
    }

    useGlobalStore().openModal(rp, (value) => {
        rp.update(value);
    });
}

const groups_modal = ref(false);
const group_list = computed(() => Object.keys(useGroupsStore().groups_list));
const open_groups_modal = () => {
    groups_modal.value = true;
};
const select_group = (group) => {
    const groups_list = useGroupsStore().groups_list;
    const g = groups_list[group];
    useGroupsStore().set_current_group(g);
    close_modal();
};
const close_modal = () => {
    groups_modal.value = false;
};
</script>

<style scoped>
    .list-area {
        overflow: auto;
        max-height: 20em;
        min-height: 5em;
    }
    .unavailable {
        color: #999;
    }
</style>
