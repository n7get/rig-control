import { computed, reactive, ref } from 'vue';
import { defineStore } from 'pinia';
import { mem_chan } from '@/js/mem_chan.js';

export const useMemChanStore = defineStore('mem_chans', () => {
    const mem_chans = reactive({});

    const get_mem_chan = (id) => {
        const chan = mem_chans[id];
        if (!chan) {
            console.warn(`Mem chan "${id}" does not exist.`);
            return null;
        }
        return chan;
    };

    const add_mem_chan = (chan) => {
        mem_chans[chan.id] = chan.asObject();
    };

    const remove_mem_chan = (id) => {
        if (!mem_chans.hasOwnProperty(id)) {
            console.warn(`Mem chan "${id}" does not exist.`);
            return;
        }
        if (id === current_mem_chan.value) {
            current_mem_chan.value = Object.keys(mem_chans).find(cid => cid !== id) || '';
            console.warn(`Current mem chan "${id}" removed, switching to "${current_mem_chan.value}".`);
        }
        delete mem_chans[id];
    };

    const current_mem_chan = ref(-1);
    const set_current_mem_chan = (name) => {
        const mc = Object.values(mem_chans).find(chan => chan.name === name);
        if (mc) {
            current_mem_chan.value = mc.id;
        } else {
            console.warn(`Mem chan "${name}" does not exist.`);
        }
    };
    const get_current_mem_chan = computed(() => {
        if (current_mem_chan.value === -1 || !mem_chans.hasOwnProperty(current_mem_chan.value)) {
            current_mem_chan.value = Object.keys(mem_chans)[0] || -1;
        }
        return mem_chans[current_mem_chan.value] || null;
    });

    return {
        mem_chans,
        get_mem_chan,
        add_mem_chan,
        remove_mem_chan,
        set_current_mem_chan,
        get_current_mem_chan,
    };
});
