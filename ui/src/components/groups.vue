<template>
    <settings-component :title="title" :list="list" />
</template>

<script setup>
import { computed, watchEffect } from 'vue';
import settingsComponent from '@/components/settings.vue';
import { useGroupsStore } from '@/stores/groups';
import { rig_property } from '@/js/rig_property';

const group_titles = useGroupsStore().group_titles;
const current_group = computed(() => useGroupsStore().current_group);

const title = computed(() => {
    return group_titles[current_group.value];
});

const list = computed(() => {
    return useGroupsStore().groups[current_group.value];
});

watchEffect(() => {
    const mode = rig_property('mode');
    useGroupsStore().set_default_group(mode.value);
});
</script>
