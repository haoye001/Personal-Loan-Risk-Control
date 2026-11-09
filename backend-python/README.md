# Python 后端（第一阶段）

## 启动

在 `backend-python` 目录执行：

```powershell
..\.venv\Scripts\python.exe -m fastapi dev app/main.py
```

启动后访问：

- 健康检查：`http://127.0.0.1:8000/health`
- Swagger 文档：`http://127.0.0.1:8000/docs`

## 当前范围

第一阶段仅实现服务启动、健康检查和不落库的注册请求校验。第二阶段会接入 MySQL、密码哈希与真实用户注册。
