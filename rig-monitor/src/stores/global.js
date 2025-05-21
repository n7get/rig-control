import { ref, computed } from 'vue'
import { defineStore } from 'pinia'

export const useGlobalStore = defineStore('global', () => {
	const ready = ref(false)
	const isReady = computed(() => {
		console.log('Checking if ready:', ready.value)
		return ready.value
	})
	const setReady = (value) => {
		console.log('Setting ready:', value)
		ready.value = value
	}

	const modal = ref({
		name: '',
		open: false,
	})

	function openModal(name) {
		modal.value.name = name
		modal.value.open = true
	}

	function closeModal() {
		modal.value.name = ''
		modal.value.open = false
	}
	
	return {
		ready, setReady, isReady, 
		modal, openModal, closeModal,
	 };
})
