
<template>
    <div class="py-3 d-flex justify-content-between w-100 border-bottom">
        <div @click="back"><u>Back</u></div>
        <div><b>Wifi Config</b></div>
        <div @click="save"><u>Save</u></div>
    </div>

    <div v-if="success_message" class="alert alert-info mt-2 d-flex justify-content-between">
        <div class="me-2 text-center"><icons-check /></div>
        <div>{{ success_message }}</div>
    </div>
    <div v-if="error_message" class="alert alert-danger mt-2 d-flex justify-content-between">
        <div class="me-2 text-center"><icons-warning /></div>
        <div>{{ error_message }}</div>
    </div>

    <b-form-group class="mt-3"
        label="AP SSID"
        label-for="ap_ssid"
        description="Enter the SSID for the Access Point (AP) mode."
    >
        <b-form-input
            id="ap_ssid"
            v-model="ap_ssid"
            placeholder="Enter AP SSID"
            required />
    </b-form-group>

    <b-form-group class="mt-3"
        label="AP Password"
        label-for="ap_password"
        description="Enter the password for the Access Point (AP) mode."
    >
        <b-form-input
            id="ap_password"
            type="password"
            v-model="ap_password"
            placeholder="Enter AP Password"
            required />
    </b-form-group>

    <b-form-group class="mt-3"
        label="STA SSID"
        label-for="sta_ssid"
        description="Enter the SSID for the Station (STA) mode."
    >
        <b-form-input
            id="sta_ssid"
            v-model="sta_ssid"
            placeholder="Enter STA SSID"
            required />
    </b-form-group>

    <b-form-group class="mt-3"
        label="STA Password"
        label-for="sta_password"
        description="Enter the password for the Station (STA) mode."
    >
        <b-form-input
            id="sta_password"
            type="password"
            v-model="sta_password"
            placeholder="Enter STA Password"
            required />
    </b-form-group>
</template>

<script setup>
import { onMounted, ref } from 'vue';
import { useRouter } from 'vue-router';
import { send_message_with_response } from '@/js/web_socket';
import iconsCheck from '@/components/icons/check.vue';
import iconsWarning from '@/components/icons/warning.vue';

const router = useRouter();

const success_message = ref('');
const error_message = ref('');
const ap_ssid = ref('');
const ap_password = ref('');
const sta_ssid = ref('');
const sta_password = ref('');

onMounted(() => {
    success_message.value = '';
    error_message.value = '';
    send_message_with_response({ topic: 'settings', event: 'get' })
        .then(response => {
            ap_ssid.value = response.ap_ssid;
            ap_password.value = response.ap_password;
            sta_ssid.value = response.sta_ssid;
            sta_password.value = response.sta_password;
        })
        .catch(error => {
            error_message.value = 'Error getting settings: ' + error;
        });
});

function save() {
    const settings = {
        ap_ssid: ap_ssid.value,
        ap_password: ap_password.value,
        sta_ssid: sta_ssid.value,
        sta_password: sta_password.value
    };

    success_message.value = '';
    error_message.value = '';
    send_message_with_response({topic: 'settings', event: 'set', value: settings})
        .then(response => {
            success_message.value = 'Settings saved successfully.';
        })
        .catch(error => {
            error_message.value = 'Error saving settings: ' + error;
        });
}

function back() {
    router.back();
}
</script>