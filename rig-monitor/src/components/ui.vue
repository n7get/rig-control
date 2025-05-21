<template>
    <div>
        <div class="col-sm-12 col-md-6 col-lg-4 px-0">
            <controls class="px-3"></controls>
            <fm-settings class="px-3"></fm-settings>
            <all-settings class="px-3"></all-settings>
            <all-menus class="px-3"></all-menus>
            <!-- <rbn></rbn> -->
            <!-- <config></config> -->
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
import controls from '@/components/controls.vue'
import { useGlobalStore } from '@/stores/global'
import settings from '@/components/settings.vue'
import allSettings from '@/components/all-settings.vue'
import fmSettings from '@/components/fm-settings.vue'
import allMenus from '@/components/all-menus.vue'
import settingsModal from '@/components/modals/settings_modal.vue';

const ready = useGlobalStore().ready;

const handleOk = (e) => {
    // uibuilder.send({topic: 'rig_status', event: 'enable_transmit', value: true});
}
</script>
<!-- <script>
module.exports = {
    components: {
        'config':       httpVueLoader('config.vue'),
        'controls':     httpVueLoader('controls.vue'),
        'menu-item':    httpVueLoader('menu-item.vue'),
        'settings':     httpVueLoader('settings.vue'),
        'rbn':          httpVueLoader("rbn.vue"),
    },
    methods: {
        handleOk: function(e) {
            uibuilder.send({topic: 'rig_status', event: 'enable_transmit', value: true});
        },
    },
    mounted: function(){
        var self = this;
        
        uibuilder.start()

        uibuilder.onChange('msg', msg => {
            var p = msg.payload;
            if(p.name === 'alert') {
                self.$bvModal.msgBoxOk(p.value, {});
            }
            else if(p.name === 'high_swr') {
                if(p.value) {
                    this.$bvModal.show('alert');
                }
                else {
                    this.$bvModal.hide('alert');
                }
            }
        });
    },
}
</script> -->
