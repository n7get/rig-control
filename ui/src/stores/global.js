import { ref, computed } from 'vue'
import { defineStore } from 'pinia'

export const useGlobalStore = defineStore('global', () => {
	const ready = ref(false)
	const isReady = computed(() => {
		return ready.value
	})
	const setReady = (value) => {
		ready.value = value
	}

	const ws_connecting = ref(false);
	const is_ws_connecting = computed(() => {
		return ws_connecting.value
	})
	const set_ws_connecting = (value) => {
		ws_connecting.value = value
	}

	const connected = ref(false)
	const is_connected = computed(() => {
		return connected.value
	})
	const set_connected = (value) => {
		connected.value = value
	}

	const has_error = ref(false)
	const error_message = ref('')
	const setError = (message) => {
		has_error.value = true
		error_message.value = message
	}
	const clearError = () => {
		has_error.value = false
		error_message.value = ''
	}

	const modal = ref({
		name: '',
		open: false,
	})
	let modal_cb = null;

	function openModal(rig_prop, cb) {
		modal.value.rig_prop = rig_prop
		modal.value.open = true
		modal_cb = cb;
	}

	function updateModal(value) {
		if (modal_cb) {
			modal_cb(value);
		}
	}

	function closeModal() {
		modal.value.name = ''
		modal.value.open = false
		modal_cb = null;
	}
	return {
		ready, setReady, isReady, 
		ws_connecting, set_ws_connecting, is_ws_connecting,
		connected, set_connected, is_connected,
		has_error, error_message, setError, clearError,
		modal, updateModal, openModal, closeModal,
	};
})
