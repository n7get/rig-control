<template>
<main>
  <div class="wrapper">
    <RouterView v-if="isReady" />
    <div class="loading wait-wrapper d-flex justify-content-center align-items-center text-center" v-else>
      <div class="spinner">
        <div class="spinner-border" role="status">
          <span class="visually-hidden">Loading...</span>
        </div>
        <div class="mt-5 fs-1 mx-4">Please wait until the controller is ready.</div>
      </div>
    </div>
  </div>
</main>
  
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
>
    <template #modal-title>
        Error
    </template>
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
</template>

<script setup>
import { computed } from 'vue';
import { useGlobalStore } from '@/stores/global';

const isReady = computed(() => useGlobalStore().ready);

const has_error = useGlobalStore().has_error;
const error_message = useGlobalStore().error_message;
const close_error = (e) => {
    useGlobalStore().clearError();
}
</script>

<style>
.wait-wrapper {
  height: 100vh;
}
</style>
