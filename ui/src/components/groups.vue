<template>
    <settings-component v-if="isReady" :title="title" :list="list" />
</template>

<script setup>
import { computed, watchEffect } from 'vue';
import settingsComponent from '@/components/settings.vue';
import { useGroupsStore } from '@/stores/groups';
import { useGlobalStore } from '@/stores/global';

const isReady = computed(() => useGlobalStore().ready);

const title = computed(() => {
    return useGroupsStore().get_current_group()
});

const list = computed(() => {
    const current_group = useGroupsStore().get_current_group();
    const list = useGroupsStore().get_group_properties(current_group);
    return list;
});

// watchEffect(() => {
//     const mode = rig_property('mode');
//     useGroupsStore().set_default_group(mode.value);
// });
</script>
