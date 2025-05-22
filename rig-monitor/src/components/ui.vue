<template>
    <div>
        <div class="col-sm-12 col-md-6 col-lg-4 px-0">
            <controls class="px-3" />
            <!-- <radio-status class="px-3" /> -->
            <mode-settings class="px-3" />
            <all-settings class="px-3" />
            <all-menus class="px-3" />
            <!-- <rbn /> -->
            <!-- <config /> -->
        </div>
  
        <settings-modal />

        <b-modal
            id="alert"
            size="sm"
            @ok="handleOk"
            no-close-on-esc
            no-close-on-backdrop
            ok-only
            hide-header-close
            header-bg-variant="danger"
            header-text-variant="light"
        >
            <template #modal-title>
                High SWR
            </template>
            <div>Transmit is disabled until OK is clicked.</div>
        </b-modal>
    </div>
</template>

<script setup>
import { rig_property } from '@/js/rig_property'

import allMenus from '@/components/all-menus.vue'
import allSettings from '@/components/all-settings.vue'
import controls from '@/components/controls.vue'
import modeSettings from './mode-settings.vue'
// import radioStatus from './radio_status.vue'
import settings from '@/components/settings.vue'
import settingsModal from '@/components/modals/settings_modal.vue';

const mode = rig_property('mode');
const handleOk = (e) => {
    // uibuilder.send({topic: 'rig_status', event: 'enable_transmit', value: true});
}
</script>
