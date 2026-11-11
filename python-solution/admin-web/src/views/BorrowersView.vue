<script setup>
import { onMounted, ref } from 'vue'
import { request } from '../api.js'

const borrowers = ref([])
const loading = ref(true)
const error = ref('')

async function load() {
  loading.value = true
  error.value = ''
  try {
    borrowers.value = await request('/admin/borrowers')
  } catch (failure) {
    error.value = failure.message
  } finally {
    loading.value = false
  }
}

onMounted(load)
</script>

<template>
  <main class="borrowers">
    <h1>借款用户</h1>
    <button :disabled="loading" @click="load">刷新</button>
    <p v-if="loading" role="status">正在加载</p>
    <p v-else-if="error" role="alert">{{ error }}</p>
    <p v-else-if="!borrowers.length">暂无借款用户</p>
    <table v-else>
      <thead><tr><th>用户编号</th><th>用户名</th></tr></thead>
      <tbody>
        <tr v-for="borrower in borrowers" :key="borrower.id">
          <td>{{ borrower.id }}</td>
          <td>{{ borrower.username }}</td>
        </tr>
      </tbody>
    </table>
  </main>
</template>

<style scoped>
.borrowers { padding: 1rem; }
table { border-collapse: collapse; margin-top: 1rem; }
th, td { padding: .6rem; border-bottom: 1px solid #ccc; text-align: left; }
[role="alert"] { color: #b42318; }
</style>
