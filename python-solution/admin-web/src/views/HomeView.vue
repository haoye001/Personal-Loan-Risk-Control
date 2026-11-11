<script setup>
import { onMounted, ref } from 'vue'
import { request } from '../api.js'

const applications = ref([])
const error = ref('')
const busy = ref(true)

async function load() {
  busy.value = true
  error.value = ''
  try {
    applications.value = await request('/admin/applications')
  } catch (failure) {
    error.value = failure.message
  } finally {
    busy.value = false
  }
}

async function review(id, decision) {
  busy.value = true
  error.value = ''
  try {
    await request('/admin/applications/' + id + '/review', { method: 'PUT', body: { decision } })
    applications.value = applications.value.filter(item => item.applicationId !== id)
  } catch (failure) {
    error.value = failure.message
  } finally {
    busy.value = false
  }
}

onMounted(load)
</script>

<template>
  <main class="applications">
    <h1>待审核申请</h1>
    <button :disabled="busy" @click="load">刷新</button>
    <p v-if="error" role="alert">{{ error }}</p>
    <p v-if="!busy && !error && !applications.length">暂无待审核申请</p>
    <table v-if="applications.length">
      <thead><tr><th>申请编号</th><th>借款人编号</th><th>产品编号</th><th>金额（元）</th><th>操作</th></tr></thead>
      <tbody>
        <tr v-for="item in applications" :key="item.applicationId">
          <td>{{ item.applicationId }}</td><td>{{ item.userId }}</td>
          <td>{{ item.loanId }}</td><td>{{ item.amount }}</td>
          <td>
            <button :disabled="busy" @click="review(item.applicationId, 'approved')">通过</button>
            <button :disabled="busy" @click="review(item.applicationId, 'rejected')">拒绝</button>
          </td>
        </tr>
      </tbody>
    </table>
  </main>
</template>

<style scoped>
.applications { padding: 1rem; }
table { border-collapse: collapse; margin-top: 1rem; }
th, td { padding: .6rem; border-bottom: 1px solid #ccc; text-align: left; }
td button { margin-right: .5rem; }
[role="alert"] { color: #b42318; }
</style>
