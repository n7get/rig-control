<template>
    <b-modal
        id="repeater-modal"
        title="Repeater Settings"
        size="sm"
        @ok="handle_ok"
        @esc="close_modal"
        @cancel="close_modal"
        v-model="open_modal"
    >
        <div>
            <b-form-group
                label="Offset:"
                label-for="offset"
            >
                <b-form-select
                    id="offset"
                    v-model="offset"
                    :options="offset_options"
                ></b-form-select>
            </b-form-group>

            <b-form-group
                label="CTCSS Mode:"
                label-for="ctcss-mode"
            >
                <b-form-select
                    id="ctcss-mode"
                    v-model="ctcss"
                    :options="ctcss_options"
                ></b-form-select>
            </b-form-group>

            <b-form-group
                label="CTCSS Tone:"
                label-for="ctcss-tone"
                v-show="ctcss === 'CTCSS ENC/DEC' || ctcss === 'CTCSS ENC'"
            >
                <b-form-select
                    id="ctcss-tone"
                    v-model="ctcss_tone_frequency"
                    :options="ctcss_tone_options"
                ></b-form-select>
            </b-form-group>

            <b-form-group
                label="DCS Tone:"
                label-for="dcs-tome"
                v-show="ctcss === 'DCS ENC/DEC' || ctcss === 'DCS ENC'"
            >
                <b-form-select
                    id="dcs-tome"
                    v-model="dcs_tone_frequency"
                    :options="dcs_tone_options"
                ></b-form-select>
            </b-form-group>
        </div>
    </b-modal>
</template>

<script setup>
import { ref } from 'vue'
import { send_command } from '@/js/web_socket.js';
import { useSettingsStore } from '@/stores/settings'
import { useGlobalStore } from '@/stores/global'
import { offset_cat, ctcss_to_cat, ctcss_tone_frequency_to_cat, cat_to_dcs } from '@/js/rig_setting.js'

const settings = useSettingsStore()
const global = useGlobalStore()

const open_modal = ref(true)

const offset = ref(settings.offset.value)
const offset_options = Object.keys(offset_cat).map((k) => ({ value: k, text: k }))

const ctcss = ref(settings.ctcss.value)
const ctcss_options = Object.keys(ctcss_to_cat).map((k) => ({ value: k, text: k }))

const ctcss_tone_frequency = ref(settings.ctcss_tone_frequency.value)
const ctcss_tone_options = Object.keys(ctcss_tone_frequency_to_cat).map((k) => ({ value: k, text: k }))

const dcs_tone_frequency = ref(settings.dcs_tone_frequency.value)
const dcs_tone_options = cat_to_dcs.map((k) => ({ value: k, text: k }))

function handle_ok() {
    if (offset.value !== settings.offset.value) {
        send_command('offset', offset.value);
    }
    if (ctcss.value !== settings.ctcss.value) {
        send_command('ctcss', ctcss.value);
    }
    if (ctcss.value === 'CTCSS ENC/DEC' || ctcss.value === 'CTCSS ENC') {
        if (ctcss_tone_frequency.value !== settings.ctcss_tone_frequency.value) {
            send_command('ctcss_tone_frequency', ctcss_tone_frequency.value);
        }
    }
    if (ctcss.value === 'DCS ENC/DEC' || ctcss.value === 'DCS ENC') {
        if (dcs_tone_frequency.value !== settings.dcs_tone_frequency.value) {
            send_command('dcs_tone_frequency', dcs_tone_frequency.value);
        }
    }

    close_modal();
}
function close_modal() {
    global.closeModal();
}
</script>
