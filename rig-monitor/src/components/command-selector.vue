<template>
    <div class="d-flex gap-3 justify-content-center">
        <b-form-checkbox
            id="form-input"
            @change="set"
            reverse switch
            size="lg"
            v-model="input_value"
        >OP MODE</b-form-checkbox>
    </div>
</template>

<script setup>
import { ref, computed, watchEffect } from 'vue';
import { useOpModeStore } from '@/stores/op_modes';
import { rig_property } from '@/js/rig_property';
import { op_mode } from '@/js/op_mode.js';

const props = defineProps({
    name: {
        type: String,
        required: true,
    },
});

const input_value = ref(false);

// const label = computed(() => {
//     const om = useOpModeStore().get_current_op_mode;
//     return om.name;
// });

watchEffect(() => {
    const com = useOpModeStore().get_current_op_mode;
    const rp = rig_property(props.name);
    input_value.value = com.commands.hasOwnProperty(rp.name);
});

const set = (e) => {
    const com = useOpModeStore().get_current_op_mode;   

    const om = op_mode.fromObject(com);
    
    if (input_value.value) {
        const rp = rig_property(props.name);
        om.commands[rp.name] = rp.value;
    } else {
        delete om.commands[props.name];
    }

    console.log('update op mode', om.asUpdate());
    om.update();
};
</script>