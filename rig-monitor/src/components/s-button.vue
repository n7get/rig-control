<template>
    <b-button size="sm" raised :pressed.sync="pressed.value" @click="click">{{ label }}</b-button>
</template>

<script setup>
import { ref } from 'vue';
import { send_command } from '@/js/web_socket';

/*
 * The following events work with this component:
 *  band_down
 *  band_up
 *  clar_clear
 *  ent_key
 *  mic_down
 *  mic_up
 *  qmb_recall
 *  qmb_store
 *  quick_split
 *  swap_vfo
 *  vfo_a_to_vfo_b
 *  vfo_b_to_vfo_a
 *  zero_in
 */

 const props = defineProps({
    event: {
        type: String,
        required: true,
    },
    label: {
        type: String,
        required: true,
    },
});

const pressed = ref(false);

function click(new_value) {
    pressed.value = true;
    window.setTimeout(function() {
        pressed.value = false;
    }, 1000);

    console.log(`Button clicked: ${props.event}`);
    send_command(props.event, new_value);
};
</script>

<style scoped>
.b-button {
    margin: 0.5rem;
}
</style>
