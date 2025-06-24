import { createMemoryHistory, createRouter } from 'vue-router'

const routes = [
  {
      path: '/',
      component: () => import('@/views/main-view.vue'),
  },
  {
    path: '/config',
    name: 'config',
    component: () => import('@/views/config-view.vue'),
  },

  {
    path: '/groups',
    name: 'groups',
    component: () => import('@/views/groups-view.vue'),
  },
  {
    path: '/groups_config',
    name: 'group_config',
    component: () => import('@/views/group-config-view.vue'),
  },
  {
    path: '/group_edit/:edit/:id?',
    name: 'group_edit',
    component: () => import('@/views/group-edit-view.vue'),
    props: true,
  },
  {
    path: '/group_export',
    name: 'group_export',
    component: () => import('@/views/group-export-view.vue'),
  },
  {
    path: '/group_import',
    name: 'group_import',
    component: () => import('@/views/group-import-view.vue'),
  },

  {
    path: '/info',
    name: 'info',
    component: () => import('@/views/info-view.vue'),
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

  {
    path: '/wifi_config',
    name: 'wifi_config',
    component: () => import('@/views/wifi-config-view.vue'),
  }
]

const router = createRouter({
  history: createMemoryHistory(),
  routes,
})

export default router
