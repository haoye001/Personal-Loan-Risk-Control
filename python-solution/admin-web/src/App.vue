<script setup>
import { ref } from 'vue'
import { login, logout, token } from './api.js'

const username = ref('')
const password = ref('')
const busy = ref(false)
const error = ref('')

async function signIn() {
  busy.value = true
  error.value = ''
  try {
    await login(username.value.trim(), password.value)
    password.value = ''
  } catch (failure) {
    error.value = failure.message
  } finally {
    busy.value = false
  }
}
</script>

<template>
  <main v-if="!token" class="login">
    <h1>管理员登录</h1>
    <form @submit.prevent="signIn">
      <label>用户名 <input v-model="username" required autocomplete="username" /></label>
      <label>密码 <input v-model="password" required type="password" autocomplete="current-password" /></label>
      <p v-if="error" role="alert">{{ error }}</p>
      <button :disabled="busy">登录</button>
    </form>
  </main>
  <template v-else>
    <nav aria-label="管理导航">
      <RouterLink to="/">贷款审批</RouterLink>
      <RouterLink to="/member">成员管理</RouterLink>
      <button type="button" @click="logout">退出登录</button>
    </nav>
    <RouterView />
  </template>
</template>

<style scoped>
nav { display: flex; gap: 1rem; align-items: center; padding: 1rem; }
.login { max-width: 24rem; margin: 3rem auto; padding: 1rem; }
form, label { display: grid; gap: .6rem; }
[role="alert"] { color: #b42318; }
</style>
