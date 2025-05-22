<template>
    <settings v-if="is_mode_active" :title="title" :list="list" />
</template>

<script setup>
import { ref, computed } from 'vue';
import { rig_property } from '@/js/rig_property';
import settings from '@/components/settings.vue';

const title = ref('');
const list = ref([]);

const mode = rig_property('mode');

const fm_mode_list = [
    'offset',
    'ctcss',
    'ctcss_tone_frequency',
    'dcs_tone_frequency',
    'mic_gain',
    'squelch_level',
];
const fm_digital_mode_list = [
    '74',
    '77',
    '75',
    '78',
    '79',
    '62',
    '76',
];
const ssb_digital_mode_list = [
    '70',
    '72',
    '73',
    '64',
    '65',
    '71',
    '66',
    '67',
    '68',
    '69',
];
const ssb_mode_list = [
    '102',
    '103',
    '104',
    '105',
    '106',
    '107',
    '108',
    '109',
    '110',
];

const mode_settings = {
    'C4FM': {
        title: 'C4FM Settings',
        list: fm_mode_list,
    },
    'DATA-FM': {
        title: 'FM Digital Settings',
        list: fm_digital_mode_list
    },
    'DATA-LSB': {
        title: 'SSB Digital Settings',
        list: ssb_digital_mode_list,
    },
    'DATA-USB': {
        title: 'SSB Digital Settings',
        list: ssb_digital_mode_list,
    },
    'FM': {
        title: 'FM Settings',
        list: fm_mode_list,
    },
    'LSB': {
        title: 'SSB Settings',
        list: ssb_mode_list,
    },
    'USB': {
        title: 'SSB Settings',
        list: ssb_mode_list,
    },
}

const is_mode_active = computed(() => {
    if (mode_settings[mode.value]) {
        title.value = mode_settings[mode.value].title;
        list.value = mode_settings[mode.value].list;
        return true;
    }
    return false;
});
</script>