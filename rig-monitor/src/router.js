import { createMemoryHistory, createRouter } from 'vue-router'

const routes = [
  {
      path: '/',
      component: () => import('@/views/ui-view.vue'),
  },
  {
      path: '/op_modes',
      name: 'op_modes',
      component: () => import('@/views/op-modes-view.vue'),
  },
  {
      path: '/op_mode_edit/:edit/:id?',
      name: 'op_mode_edit',
      component: () => import('@/views/op-mode-edit-view.vue'),
      props: true,
  },
  {
    path: '/mem_chans',
    name: 'mem_chans',
    component: () => import('@/views/mem-chans-view.vue'),
  },
  {
    path: '/mem_chan_edit/:edit/:id?',
    name: 'mem_chan_edit',
    component: () => import('@/views/mem-chan-edit-view.vue'),
    props: true,
  },
]

const router = createRouter({
  history: createMemoryHistory(),
  routes,
})

export default router
