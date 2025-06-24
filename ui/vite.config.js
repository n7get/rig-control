import { fileURLToPath, URL } from 'node:url'

import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'
import Components from 'unplugin-vue-components/vite'
import {BootstrapVueNextResolver} from 'bootstrap-vue-next'
import { compression } from 'vite-plugin-compression2'

// https://vite.dev/config/
export default defineConfig({
  server: {
    host: '0.0.0.0',
  },
  plugins: [
    vue(),
    vueDevTools(),
    Components({
      resolvers: [BootstrapVueNextResolver()],
    }),
    compression({
      algorithms: [
        'gzip',
      ],
      deleteOriginalAssets: true,
    }),
  ],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    },
  },
  build: {
    cssMinify: true,
    emptyOutDir: true,
    rollupOptions: {
      output: {
        // You can specify the output directory for your assets
        dir: 'dist/', 
        
        // Customize the naming pattern for entry files (your main JS/CSS files)
        entryFileNames: 'entry-[name].js', 
        
        // Customize the naming pattern for other assets (images, fonts, etc.)
        assetFileNames: 'assets/[name].[ext]', 
        
        // Customize the naming pattern for code-split chunks
        chunkFileNames: 'chunk-[hash].js', 
      },
    },
  },
});