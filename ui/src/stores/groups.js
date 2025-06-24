import { ref, computed } from 'vue'
import { defineStore } from 'pinia'
import { get_all_menus_list, rig_property } from '@/js/rig_property.js';
import { get_all_settings_list } from '@/js/rig_property.js';
import { useOpModeStore } from '@/stores/op_modes';

const OP_MODE = 'Op Mode';
const ALL_MENUS = 'All Menus';
const ALL_SETTINGS = 'All Settings';
const NO_CURRENT_GROUP = '';

export const useGroupsStore = defineStore('groups', () => {
    const groups = ref({})

    function add_group(group) {
        groups.value[group.name] = group.asObject();
    }
    function remove_group(id) {
        const group = Object.values(groups.value).find((g) => g.id === id);
        if (group) {
            delete groups.value[group.name];
        }
    }
    function find_group(id) {
        return Object.values(groups.value).find((g) => g.id === id);
    }

    function get_group_properties(name) {
        if (name === OP_MODE) {
            const current_op_modes = useOpModeStore().get_current_op_mode;
            if (current_op_modes) {
                return current_op_modes.commands.map(cmd => cmd.name);
            }

            return [];
        }
        if (name === ALL_MENUS) {
            return get_all_menus_list();
        }
        if (name === ALL_SETTINGS) {
            return get_all_settings_list();
        }
        if (groups.value[name]) {
            return groups.value[name].properties;
        }
        console.warn(`Group "${name}" does not exist.`);
        return null;
    }
    
    const groups_list = computed(() => {
        const mode = rig_property('mode').value;

        const list = Object.values(groups.value)
            .filter(g => g.modes.some((gm) => gm === mode))
            .sort((a, b) => a.order - b.order)
            .map(g => g.name);

        list.push(OP_MODE);
        list.push(ALL_MENUS);
        list.push(ALL_SETTINGS);

        return list;
    });

    const current_group = ref(NO_CURRENT_GROUP);
    function get_current_group() {
        const list = groups_list.value;

        if (current_group.value === NO_CURRENT_GROUP || !list.includes(current_group.value)) {
            if (list.length > 0) {
                return list[0];
            }
            return ALL_SETTINGS;
        }
        return current_group.value;
    }
    function set_current_group(name) {
        if ([OP_MODE, ALL_MENUS, ALL_SETTINGS].includes(name)) {
            current_group.value = name;
            return;
        }

        if (groups.value[name]) {
            current_group.value = name;
            return;
        }

        console.warn(`Group "${name}" does not exist.`);
    }
    function reset_current_group() {
        current_group.value = NO_CURRENT_GROUP;
    }

    return {
        groups,
        add_group, remove_group, find_group, 
        groups_list,
        get_group_properties,
        get_current_group, set_current_group, reset_current_group,
    };
});
