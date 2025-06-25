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
        no-header-close
        header-bg-variant="danger"
        header-text-variant="light"
        v-model="has_error"
        title="Error!!"
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
        no-header-close
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

<b-modal
      id="not-connected"
      no-close-on-esc
      no-close-on-backdrop
      ok-title="Connect"
      ok-only
      @ok="connect"
      no-header-close
      header-bg-variant="danger"
      header-text-variant="light"
      v-model="not_connected"
      title="Not Connected"
  >
      <div class="fs-4">
        Connection to the controller has been lost.
        Tap <b>Connect</b> to attempt to reconnect.
      </div>
  </b-modal>
</template>

<script setup>
import { computed } from 'vue';
import { useGlobalStore } from '@/stores/global';
import { connect_ws } from '@/js/web_socket.js';

const has_error = computed(() => useGlobalStore().has_error);
const error_message = computed(() => useGlobalStore().error_message);
const not_connected = computed(() => !useGlobalStore().isConnected);

const close_error = (e) => {
    useGlobalStore().clearError();
}

function connect() {
    connect_ws();
}
</script>

<style>
.wait-wrapper {
  height: 100vh;
}
</style>
