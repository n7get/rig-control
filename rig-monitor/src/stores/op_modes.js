import { computed, reactive, ref } from 'vue'
import { defineStore } from 'pinia'
import { useGroupsStore } from '@/stores/groups.js';

export const useOpModeStore = defineStore('op_modes', () => {
    const op_modes = reactive({});

    const get_op_mode = (id) => {
        const op_mode = op_modes[id];
        if (!op_mode) {
            console.warn(`Op mode "${id}" does not exist.`);
            return null;
        }
        return op_mode;
    }
    const add_op_mode = (op_mode) => {
        op_modes[op_mode.id] = op_mode.asObject();
        useGroupsStore().groups_init_op_modes();
    }
    const remove_op_mode = (id) => {
        if (!op_modes.hasOwnProperty(id)) {
            console.warn(`Op mode "${id}" does not exist.`);
            return;
        }
        if (id === current_op_mode.value) {
            current_op_mode.value = Object.keys(op_modes).find(id => id !== id) || '';
            console.warn(`Current op mode "${id}" removed, switching to "${current_op_mode.value}".`);
        }
        delete op_modes[id];
    }

    const current_op_mode = ref(-1);
    const set_current_op_mode = (name) => {
        const om = Object.values(op_modes).find(op_mode => op_mode.name === name);
        if (om) {
            current_op_mode.value = om.id;
            useGroupsStore().groups_init_op_modes();
        } else {
            console.warn(`Op mode "${name}" does not exist.`);
        }
    }
    const get_current_op_mode = computed(() => {
        if (current_op_mode.value === -1 || !op_modes.hasOwnProperty(current_op_mode.value)) {
            current_op_mode.value = Object.keys(op_modes)[0] || -1;
        }
        return op_modes[current_op_mode.value] || null;
    });
    return {
        op_modes,
        get_op_mode,
        add_op_mode,
        remove_op_mode,
        set_current_op_mode,
        get_current_op_mode,
    };
});
