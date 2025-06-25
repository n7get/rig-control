import { createApp } from 'vue'
import { createBootstrap } from 'bootstrap-vue-next'
import { createPinia } from 'pinia'
import { connect_ws } from '@/js/web_socket.js'
import router from '@/router'
import App from '@/App.vue'
import 'bootstrap/dist/css/bootstrap.css'
import 'bootstrap-vue-next/dist/bootstrap-vue-next.css'

const app = createApp(App)

app.use(createPinia())
app.use(createBootstrap())
app.use(router)

connect_ws();

app.mount('#app')
