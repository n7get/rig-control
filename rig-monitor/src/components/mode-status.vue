
<template>
    <div>
        <span @click="handleClick">{{ mode.value     }}</span>

        <b-modal
            id="new-mode"
            size="sm"
            hide-footer
            v-model="show_modal"
        >
            <template #modal-title>
                Set mode
            </template>
            <div class="font-weight-bold my-1 ml-0 py-3 pl-3 border text-light bg-primary" v-for="m in modeOptions" @click="selectMode(m)">&nbsp;&nbsp;{{ m }}</div>
        </b-modal>
    </div>
</template>

<script setup>
import { computed, ref } from 'vue';
import { cat_modes, modes_cat } from '../js/rig_setting.js';
import { send_command } from '@/js/web_socket.js';
import { useSettingsStore } from '@/stores/settings';

const mode = useSettingsStore().settings.mode;
const modeOptions = computed(() => {
    return Object.keys(modes_cat).map(m => {
        return m;
    });
});

const show_modal = ref(false);
const selectMode = (m) => {
    show_modal.value = false;
    send_command('mode', m);
};
const handleClick = (e) => {
    show_modal.value = true;
};
// module.exports = {
//     data: function() {
//         return {
//             mode: 'mode',
//             modeOptions: this.$noderig.modeOptions,
//         }
//     },
//     methods: {
//         handleClick: function() {
//             this.$bvModal.show('new-mode');
//         },
//         selectMode: function(m) {
//             this.$nextTick(function() {
//                 this.$bvModal.hide('new-mode');
//             });

//             uibuilder.send({topic: 'mode', event: 'set', value: m});
//         },
//     },
//     mounted: function() {
//         var self = this;

//         uibuilder.onChange('msg', msg => {
//             var p = msg.payload;

//             if(p.name === 'mode') {
//                 if(p.hasOwnProperty('value')) {
//                     self.mode = p.value;
//                 }
//             }
//         });
//     }
// }
</script>
