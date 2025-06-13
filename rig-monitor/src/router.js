import { createMemoryHistory, createRouter } from 'vue-router'

const routes = [
  {
      path: '/',
      component: () => import('@/views/ui-view.vue'),
  },
  {
    path: '/config',
    name: 'config',
    component: () => import('@/views/config-view.vue'),
  },

  {
    path: '/mem_chans',
    name: 'mem_chans',
    component: () => import('@/views/mem-chans-view.vue'),
  },
  {
    path: '/mem_chan_config',
    name: 'mem_chan_config',
    component: () => import('@/views/mem-chan-config-view.vue'),
  },
  {
    path: '/mem_chan_edit/:edit/:id?',
    name: 'mem_chan_edit',
    component: () => import('@/views/mem-chan-edit-view.vue'),
    props: true,
  },
  {
    path: '/mem_chan_export',
    name: 'mem_chan_export',
    component: () => import('@/views/mem-chan-export-view.vue'),
  },
  {
    path: '/mem_chan_import',
    name: 'mem_chan_import',
    component: () => import('@/views/mem-chan-import-view.vue'),
  },

  {
      path: '/op_modes',
      name: 'op_modes',
      component: () => import('@/views/op-modes-view.vue'),
  },
  {
    path: '/op_mode_config',
    name: 'op_mode_config',
    component: () => import('@/views/op-mode-config-view.vue'),
  },
  {
      path: '/op_mode_edit/:edit/:id?',
      name: 'op_mode_edit',
      component: () => import('@/views/op-mode-edit-view.vue'),
      props: true,
  },
  {
    path: '/op_mode_export',
    name: 'op_mode_export',
    component: () => import('@/views/op-mode-export-view.vue'),
  },
  {
    path: '/op_mode_import',
    name: 'op_mode_import',
    component: () => import('@/views/op-mode-import-view.vue'),
  },
]

const router = createRouter({
  history: createMemoryHistory(),
  routes,
})

export default router
