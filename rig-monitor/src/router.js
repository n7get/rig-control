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
  {
    path: '/mem_chans',
    name: 'mem_chans',
    component: () => import('@/view/mem-chans-view.vue'),
  },
  {
    path: '/mem_chan_edit/:edit/:id?',
    name: 'mem_chan_edit',
    component: () => import('@/view/mem-chan-edit-view.vue'),
    props: true,
  },
]

const router = createRouter({
  history: createMemoryHistory(),
  routes,
})

export default router
