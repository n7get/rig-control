<template>
  <div>
    <h1>WebSocket Example</h1>
    <div>
      <p>S-METER READING: {{ s_meter_reading.value }}</p>
      <p>VFO A: {{ vfo_a.value }}</p>
    </div>
    <div>
        <s_meter></s_meter>
    </div>
  </div>
</template>

<script setup>
import { useSettingsStore } from '@/stores/settings';
import s_meter from '../components/s-meter-bootstrap-progressbar.vue';

const s_meter_reading = useSettingsStore().s_meter_reading;
const vfo_a = useSettingsStore().vfo_a;
</script>

<style scoped>
button {
  margin-top: 10px;
  padding: 10px 20px;
  background-color: #007bff;
  color: white;
  border: none;
  border-radius: 5px;
  cursor: pointer;
}

button:hover {
  background-color: #0056b3;
}
</style>