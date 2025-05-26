<template>
<main>
  <div class="wrapper">
    <ui v-if="isReady"></ui>
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
</template>

<script setup>
import { computed } from 'vue';
import { useGlobalStore } from '@/stores/global';
import ui from './components/ui.vue'

const isReady = computed(() => useGlobalStore().ready);
</script>

<style>
.wait-wrapper {
  height: 100vh;
}
</style>