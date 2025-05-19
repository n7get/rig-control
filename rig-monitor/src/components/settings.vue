<template>
    <b-card class="pb-2" no-body border-variant="secondary">
        <b-card-header class="px-2 d-flex justify-content-between align-items-left">
            <div @click="toggleShowSettings" text-variant="white" bg-variant="secondary">
                 Settings
            </div>
        </b-card-header>
        <div v-show="show_settings" flush>
            <div class="px-2 d-flex justify-content-between align-items-left bg-secondary text-light">
                <div>Description</div>
                <div>Value</div>
            </div>
            <div class="list-area">
                <div class="px-2 d-flex justify-content-between align-items-left"
                    v-for="(s, name) in settings"
                    :key="name"
                    @click="editSetting(name)">

                    <template v-if="!s.unavailable && s.all">
                        <div>{{ s.desc }}<span v-if="s.changed">*</span></div>
                        <div>{{ s.value }}</div>
                    </template>
                </div>
            </div>
        </div>
    </b-card>
</template>

<script setup>
import { computed, ref } from 'vue';
import { send_command } from '@/js/web_socket.js';
import { useGlobalStore } from '@/stores/global';
import { useSettingsStore } from '@/stores/settings';

const show_settings = ref(true);
const settings = useSettingsStore();
const boolean_settings = [
    'break_in',
    'lock',
    'keyer',
    'manual_notch',
    'monitor',
    'mox_set',
    'narrow',
    'noise_blanker',
    'noise_reduction',
    'parametric_microphone_equalizer',
    'preamp',
    'rf_attenuator',
    'speech_processor',
    'transmit',
    'tx_clar',
    'txw',
    'vox',
];

const toggleShowSettings = () => {
    show_settings.value = !show_settings.value;
}

const editSetting = (name) => {
    if (name === 'antenna_tuner_control') {
        send_command('antenna_tuner_control', settings[name].value === 'on' ? 'off' : 'on');
        return;
    }
    if (boolean_settings.includes(name)) {
        toggle_boolean(name);
        return;
    }
    useGlobalStore().openModal(name);
}

const toggle_boolean = (name) => {
    const value = !settings[name].value;
    console.log(`Toggling setting ${name} to ${value}`);
    send_command(name, value);
}

// module.exports = {
//     data: function() {
//         return {
//             show_settings: true,
//             display_mode: 'fav',
//             edit_mode: false,
//             show_changed_only: false,
//             settings: this.$noderig.settings,
//         };
//     },
//     methods: {
//         toggleShowSettings: function() {
//             this.show_settings = !this.show_settings;
//         },
//         toggleDisplayMode: function() {
//             switch(this.display_mode) {
//             case 'all':     this.display_mode = 'saved'; break;
//             case 'changed': this.display_mode = 'fav'; break;
//             case 'fav':     this.display_mode = 'all'; break;
//             case 'saved':   this.display_mode = 'changed'; break;
//             }
//         },
//         show_display_mode: function() {
//             switch(this.display_mode) {
//             case 'all':     return 'All';
//             case 'changed': return 'Changed';
//             case 'fav':     return 'Fav';
//             case 'saved':   return 'Saved';
//             }
//             return 'Unknown';
//         },
//         showConfigSetting: function(s) {
//             switch(this.display_mode) {
//             case 'all':     return true;
//             case 'changed': return s.changed;
//             case 'fav':     return s.fav;
//             case 'saved':   return s.saved;
//             }
//             return false;
//         },
//         edit: function() {
//             this.edit_mode = true;
//         },
//         done: function() {
//             this.edit_mode = false;
//         },
//         toggleFav: function(name) {
//             uibuilder.send({topic: 'settings', event: 'toggle_fav', value: name});
//         },
//         togglesaved: function(name) {
//             uibuilder.send({topic: 'settings', event: 'toggle_saved', value: name});
//         },
//     },
//     mounted: function() {
//         var self = this;

//         uibuilder.onChange('msg', msg => {
//             var p = msg.payload;

//             if(self.settings.hasOwnProperty(p.name)) {
//                 if(p.hasOwnProperty('changed')) {
//                     self.settings[p.name].changed = p.changed;
//                 }
//                 if(p.hasOwnProperty('fav')) {
//                     self.settings[p.name].fav = p.fav;
//                 }
//                 if(p.hasOwnProperty('saved')) {
//                     self.settings[p.name].saved = p.saved;
//                 }
//                 if(p.hasOwnProperty('value')) {
//                     self.settings[p.name].value = p.value;
//                 }
//             }
//         });
//     }
// }
</script>

<style scoped>
    .list-area {
        overflow: auto;
        max-height: 20em;
        min-height: 5em;
    }
</style>
