<template>
  <div class="container">
    <h2 class="title">成员管理</h2>
    <button class="btn primary" @click="openDialog">新增成员</button>

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
            <button class="btn edit" @click="editRow(item)">编辑</button>
            <button class="btn del" @click="delRow(item)">删除</button>
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
        <div class="btn-group">
          <button class="btn cancel" @click="dialogVisible = false">取消</button>
          <button class="btn primary" @click="submit">保存</button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'

const tableData = ref([
  { id: 1, name: '张三', account: 'zhangsan', phone: '13800138000', role: '普通成员' },
  { id: 2, name: '李四', account: 'lisi', phone: '13900139000', role: '管理员' }
])

const dialogVisible = ref(false)
const form = ref({ id: null, name: '', account: '', phone: '', role: '' })

const openDialog = () => {
  form.value = { id: null, name: '', account: '', phone: '', role: '' }
  dialogVisible.value = true
}

const editRow = (row) => {
  form.value = { ...row }
  dialogVisible.value = true
}

const delRow = (row) => {
  const idx = tableData.value.findIndex(item => item.id === row.id)
  if (idx !== -1) tableData.value.splice(idx, 1)
}

const submit = () => {
  if (form.value.id) {
    const target = tableData.value.find(i => i.id === form.value.id)
    if (target) Object.assign(target, form.value)
  } else {
    const maxId = tableData.value.length ? Math.max(...tableData.value.map(i => i.id)) : 0
    tableData.value.push({ ...form.value, id: maxId + 1 })
  }
  dialogVisible.value = false
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