<template>
  <div class="container">
    <h2 class="title">成员管理</h2>
    <p v-if="error && !dialogVisible" role="alert">{{ error }}</p>
    <button class="btn primary" :disabled="busy" @click="openDialog">新增成员</button>

    <table class="table">
      <thead>
        <tr>
          <th>ID</th>
          <th>姓名</th>
          <th>账号</th>
          <th>手机号</th>
          <th>角色</th>
          <th>操作</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="item in tableData" :key="item.id">
          <td>{{ item.id }}</td>
          <td>{{ item.name }}</td>
          <td>{{ item.account }}</td>
          <td>{{ item.phone }}</td>
          <td>{{ item.role }}</td>
          <td>
            <button class="btn edit" :disabled="busy" @click="editRow(item)">编辑</button>
            <button class="btn del" :disabled="busy" @click="delRow(item)">删除</button>
          </td>
        </tr>
      </tbody>
    </table>

    <!-- 弹窗表单 -->
    <div class="mask" v-if="dialogVisible">
      <div class="dialog">
        <h3>{{ form.id ? '编辑成员' : '新增成员' }}</h3>
        <div class="form-item">
          <label>姓名</label>
          <input v-model="form.name" type="text" placeholder="请输入姓名" />
        </div>
        <div class="form-item">
          <label>账号</label>
          <input v-model="form.account" type="text" placeholder="请输入账号" />
        </div>
        <div class="form-item">
          <label>手机号</label>
          <input v-model="form.phone" type="text" placeholder="请输入手机号" />
        </div>
        <div class="form-item">
          <label>角色</label>
          <input v-model="form.role" type="text" placeholder="请输入角色" />
        </div>
        <p v-if="error" role="alert">{{ error }}</p>
        <div class="btn-group">
          <button class="btn cancel" :disabled="busy" @click="dialogVisible = false">取消</button>
          <button class="btn primary" :disabled="busy" @click="submit">保存</button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { onMounted, ref } from 'vue'
import { request } from '../api.js'

defineOptions({ name: 'MemberDirectory' })

const tableData = ref([])
const dialogVisible = ref(false)
const form = ref({ id: null, name: '', account: '', phone: '', role: '' })
const busy = ref(false)
const error = ref('')

async function run(action) {
  busy.value = true
  error.value = ''
  try {
    await action()
  } catch (failure) {
    error.value = failure.message
  } finally {
    busy.value = false
  }
}

onMounted(() => run(async () => {
  tableData.value = await request('/admin/members')
}))

function openDialog() {
  form.value = { id: null, name: '', account: '', phone: '', role: '' }
  dialogVisible.value = true
}

function editRow(row) {
  form.value = { ...row }
  dialogVisible.value = true
}

function delRow(row) {
  if (!window.confirm('删除成员 ' + row.name + '？')) return
  return run(async () => {
    await request('/admin/members/' + row.id, { method: 'DELETE' })
    tableData.value = tableData.value.filter(item => item.id !== row.id)
  })
}

function submit() {
  return run(async () => {
    const { id, ...body } = form.value
    const saved = await request(id ? '/admin/members/' + id : '/admin/members', {
      method: id ? 'PUT' : 'POST', body,
    })
    tableData.value = id
      ? tableData.value.map(item => item.id === id ? saved : item)
      : [...tableData.value, saved]
    dialogVisible.value = false
  })
}
</script>

<style scoped>
.container {
  padding: 30px;
  max-width: 1100px;
  margin: 0 auto;
  font-family: "Microsoft Yahei", sans-serif;
}
.title {
  color: #2c3e50;
  margin-bottom: 20px;
}
.btn {
  padding: 6px 14px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 14px;
}
.primary {
  background-color: #409eff;
  color: white;
}
.edit {
  background-color: #67c23a;
  color: white;
  margin-right: 6px;
}
.del {
  background-color: #f56c6c;
  color: white;
}
.cancel {
  background-color: #909399;
  color: white;
}
.table {
  width: 100%;
  border-collapse: collapse;
  margin-top: 16px;
}
.table th, .table td {
  border: 1px solid #e4e7ed;
  padding: 12px;
  text-align: center;
}
.table th {
  background-color: #f5f7fa;
  color: #606266;
}
.mask {
  position: fixed;
  inset: 0;
  background: rgba(0,0,0,0.4);
  display: flex;
  align-items: center;
  justify-content: center;
}
.dialog {
  width: 420px;
  background: white;
  padding: 24px;
  border-radius: 8px;
}
.form-item {
  margin-bottom: 14px;
}
.form-item label {
  display: block;
  margin-bottom: 4px;
  color: #333;
}
.form-item input {
  width: 100%;
  box-sizing: border-box;
  padding: 8px 10px;
  border: 1px solid #dcdfe6;
  border-radius: 4px;
}
.btn-group {
  margin-top: 20px;
  text-align: right;
}
.btn-group .btn {
  margin-left: 8px;
}
</style>
