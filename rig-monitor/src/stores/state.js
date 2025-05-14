import { ref, computed } from 'vue'
import { defineStore } from 'pinia'

export const useStateStore = defineStore('state', () => {
	const ready = ref(false)

	const setReady = (value) => {
		ready.value = value
	}
	const isReady = computed(() => {
		return ready.value
	})

	return { ready, setReady, isReady }
})
