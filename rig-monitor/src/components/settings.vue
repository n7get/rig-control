<template>
    <b-card class="pb-2" no-body border-variant="secondary">
        <b-card-header class="px-2 d-flex justify-content-center align-items-left" @click="toggleShowSettings">
            <div text-variant="white" bg-variant="secondary">
                 {{ title }}
            </div>
        </b-card-header>
        <div v-show="show_settings" flush>
            <div class="px-2 d-flex justify-content-between align-items-left bg-secondary text-light">
                <div>Description</div>
                <div>Value</div>
            </div>
            <div class="list-area">
                <div class="px-2 d-flex justify-content-between align-items-left"
                    v-for="rp in prop_list"
                    :key="rp.name"
                    @click="editSetting(rp)">

                    <div :class="{ unavailable: rp.unavailable }">{{ rp.desc }}<span v-if="rp.changed">*</span></div>
                    <div :class="{ unavailable: rp.unavailable }">{{ rp.value }}{{ rp.suffix }}</div>
                </div>
            </div>
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
const prop_list = get_property_list(props.list);
const show_settings = ref(props.open === 'true');

const toggleShowSettings = () => {
    show_settings.value = !show_settings.value;
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
