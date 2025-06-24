<template>
<div id="app-view" class="d-flex justify-content-center w-100">
  <div class="w-sm-100 col-12 col-md-6 col-lg-4 px-3">
    <RouterView />
    
    <settings-modal />

    <b-modal
        id="error"
        size="sm"
        @ok="close_error"
        no-close-on-esc
        no-close-on-backdrop
        ok-only
        hide-header-close
        header-bg-variant="danger"
        header-text-variant="light"
        v-model="has_error"
        titele="Error!!"
    >
        <div>{{ error_message }}</div>
    </b-modal>

    <!-- <b-modal
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
    </b-modal> -->
  </div> 
</div>
</template>

<script setup>
import { computed } from 'vue';
import { useGlobalStore } from '@/stores/global';

const has_error = computed(() => useGlobalStore().has_error);
const error_message = computed(() => useGlobalStore().error_message);
const close_error = (e) => {
    useGlobalStore().clearError();
}
</script>

<style>
.wait-wrapper {
  height: 100vh;
}
</style>
