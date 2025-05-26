<template>
    <b-card class="pb-2" no-body border-variant="secondary">
        <b-card-header class="d-flex justify-content-center" @click="choose_group">
            <div text-variant="white" bg-variant="secondary">
                 {{ title }}
            </div>
        </b-card-header>
        <div flush>
            <div class="px-2 d-flex justify-content-between align-items-left bg-secondary text-light">
                <div>Description</div>
                <div>Value</div>
            </div>
            <b-list-group>
                <b-list-group-item class="px-2 d-flex justify-content-between align-items-left"
                    v-for="rp in prop_list"
                    :key="rp.name"
                    @click="editSetting(rp)">

                    <div :class="{ unavailable: rp.unavailable }">{{ rp.desc }}<span v-if="rp.changed">*</span></div>
                    <div :class="{ unavailable: rp.unavailable }">{{ rp.value }}{{ rp.suffix }}</div>
                </b-list-group-item>
            </b-list-group>
        </div>
    </b-card>
</template>

<script setup>
import { computed, ref } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { boolean_settings, get_property_list } from '@/js/rig_property.js';

const props = defineProps({
    list: {
        type: Object,
        required: true,
    },
    open: {
        type: String,
        default: 'true',
    },
    title: {
        type: String,
        default: 'Settings',
    },
});

const global = useGlobalStore();
const show_settings = ref(props.open === 'true');

const prop_list = computed(() => get_property_list(props.list));

const choose_group = () => {
    useGlobalStore().openModal('groups');
}

const editSetting = (rp) => {
    if (rp.unavailable) {
        return;
    }

    if (rp.name === 'antenna_tuner_control') {
        rp.update(rp.value === 'on' ? 'off' : 'on');
        return;
    }

    if (boolean_settings.includes(rp.name)) {
        toggle_boolean(rp);
        return;
    }
    useGlobalStore().openModal(rp.name);
}

const toggle_boolean = (rp) => {
    const value = !rp.value;
    console.log(`Toggling setting ${rp.name} to ${value}`);
    rp.update(value);
}
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
