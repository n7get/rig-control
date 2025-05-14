import { createApp } from 'vue'
import { createBootstrap } from 'bootstrap-vue-next'
import { createPinia } from 'pinia'
import { connect_ws } from './js/web_socket.js'

import App from './App.vue'

import 'bootstrap/dist/css/bootstrap.css'
import 'bootstrap-vue-next/dist/bootstrap-vue-next.css'

const app = createApp(App)

app.use(createPinia())
app.use(createBootstrap())

connect_ws();

app.mount('#app')
