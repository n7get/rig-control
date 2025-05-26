import { ref, computed } from 'vue'
import { defineStore } from 'pinia'
import { get_all_menus_list, rig_property } from '@/js/rig_property.js';
import { get_all_settings_list } from '@/js/rig_property.js';

export const useGroupsStore = defineStore('groups', () => {
    const groups = ref({})

    const current_group = ref('SETTINGS');

    const set_default_group = (mode) => {
        switch (mode) {
        case 'AM':
            current_group.value = 'AM';
            break;
        case 'CW':
            current_group.value = 'CW';
            break;
        case 'FM':
            current_group.value = 'FM';
            break;
        case 'DATA-FM':
            current_group.value = 'DATA-FM';
            break;
        case 'LSB':
        case 'USB':
            current_group.value = 'SSB';
            break;
        case 'DATA-LSB':
        case 'DATA-USB':
            current_group.value = 'DATA-SSB';
            break;
        default:
            current_group.value = 'SETTINGS';
            break;
        }
    }

    const make_groups_list = (mode) => {
        const list = {};

        switch (mode) {
            case 'AM':
                list['AM Settings'] = 'AM';
                list['VOX Settings'] = 'VOX';
                break;
            case 'CW':
                list['CW Settings'] = 'CW';
                break;
            case 'FM':
                list['FM Settings'] = 'FM';
                list['VOX Settings'] = 'VOX';
                break;
            case 'DATA-FM':
                list['Digital Settings'] = 'DATA-FM';
                break;
            case 'LSB':
            case 'USB':
                list['SSB Settings'] = 'SSB';
                list['VOX Settings'] = 'VOX';
                break;
            case 'DATA-LSB':
            case 'DATA-USB':
                list['Digital Settings'] = 'DATA-SSB';
                break;
            default:
                break;
        }

        list['All Menus'] = 'MENUS';
        list['All Settings'] = 'SETTINGS';

        return list;
    }
    
    const groups_list = computed(() => {
        const mode = rig_property('mode');
        return make_groups_list(mode.value);
    });

    const groups_init = () => {
        groups.value['MENUS'] = get_all_menus_list();
        groups.value['SETTINGS'] = get_all_settings_list();

        groups.value['AM'] = [
            'mic_gain',
            'squelch_level',
            '41',
            '42',
            '43',
            '44',
            '45',
            '46',
            '47',
            '48',
            '49',
        ];
        groups.value['CW'] = [
            'break_in',
            'cw_break_in_delay_time',
            'cw_spot',
            'key_pitch',
            'key_speed',
            'keyer',
            '50',
            '51',
            '52',
            '53',
            '54',
            '55',
            '56',
            '57',
            '58',
            '59',
        ];
        groups.value['FM'] = [
            'offset',
            'ctcss',
            'ctcss_tone_frequency',
            'dcs_tone_frequency',
            'mic_gain',
            'squelch_level',
        ];
        groups.value['DATA-FM'] = [
            '74',
            '77',
            '75',
            '78',
            '79',
            '62',
            '76',
        ];
        groups.value['DATA-SSB'] = [
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
        groups.value['SSB'] = [
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
        groups.value['VOX'] = [
            'vox',
            'vox_delay_time',
            'vox_gain',
            '142',
            '143',
            '144',
            '145',
            '146',
            '147',
            '148',
        ];
    }

    const group_titles = {
        'AM': 'AM Settings',
        'CW': 'CW Settings',
        'FM': 'FM Settings',
        'DATA-FM': 'Digital Settings',
        'SSB': 'SSB Settings',
        'DATA-SSB': 'Digital Settings',
        'MENUS': 'All Menus',
        'SETTINGS': 'All Settings',
        'VOX': 'VOX Settings',
    }
    
    const set_current_group = (group) => {
        if (groups.value[group]) {
            current_group.value = group;
        } else {
            console.warn(`Group "${group}" does not exist.`);
        }
    }

    return {
        groups,
        groups_init,
        groups_list,
        group_titles,
        current_group,
        set_current_group,
        set_default_group,
    }
});