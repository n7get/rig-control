import { createMemoryHistory, createRouter } from 'vue-router'

import ui from '@/view/ui-view.vue'
import opModes from '@/view/op-modes-view.vue'
import opModesEdit from '@/view/op-mode-edit-view.vue'

const routes = [
  {
      path: '/',
      component: ui,
  },
  {
      path: '/op_modes',
      name: 'op_modes',
      component: opModes,
  },
  {
      path: '/op_mode_edit/:edit/:id?',
      name: 'op_mode_edit',
      component: opModesEdit,
      props: true,
  },
]

const router = createRouter({
  history: createMemoryHistory(),
  routes,
})

export default router
