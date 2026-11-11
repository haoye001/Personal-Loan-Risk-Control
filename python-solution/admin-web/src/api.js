import { ref } from 'vue'

const tokenKey = 'loan-admin-token'
export const token = ref(sessionStorage.getItem(tokenKey) || '')

export function logout() {
  token.value = ''
  sessionStorage.removeItem(tokenKey)
}

export async function request(path, { method = 'GET', body, accessToken = token.value } = {}) {
  const headers = {}
  if (accessToken) headers.Authorization = 'Bearer ' + accessToken
  if (body !== undefined) headers['Content-Type'] = 'application/json'
  const base = (import.meta.env.VITE_API_BASE_URL || '').replace(/\/$/, '')
  const response = await fetch(base + '/api' + path, {
    method, headers, body: body === undefined ? undefined : JSON.stringify(body),
  })
  if (response.status === 204) return
  const data = await response.json().catch(() => null)
  if (response.status === 401 && accessToken && accessToken === token.value) logout()
  if (!response.ok) {
    const detail = data?.detail
    throw new Error(Array.isArray(detail)
      ? detail.map(item => item.loc.at(-1) + '：' + item.msg).join('；')
      : detail || '请求失败（HTTP ' + response.status + '）')
  }
  return data
}

export async function login(username, password) {
  const result = await request('/auth/login', {
    method: 'POST', body: { username, password }, accessToken: '',
  })
  const profile = await request('/users/me', { accessToken: result.access_token })
  if (profile.role !== 'admin') throw new Error('请使用管理员账号登录')
  sessionStorage.setItem(tokenKey, result.access_token)
  token.value = result.access_token
}
