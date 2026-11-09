# 个人贷款风控系统 · FastAPI 后端

## 本地启动

从 `backend-python` 目录执行以下 PowerShell 命令，需要 Python 3.11 或更高版本。

```powershell
python -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
Copy-Item .env.example .env
.\.venv\Scripts\python.exe -c "import secrets; print(secrets.token_urlsafe(48))"
```

将生成的值填入 `.env` 的 `JWT_SECRET`。默认使用本地 SQLite 文件 `loan.db`。使用 MySQL 时，先创建空数据库和应用账户，再填写 `DATABASE_URL`，格式见 `.env.example`。前端跨域时，在 `CORS_ORIGINS` 写入允许的来源，用逗号分隔。

```powershell
.\.venv\Scripts\python.exe -m alembic upgrade head
.\.venv\Scripts\python.exe -m app.manage seed-products
.\.venv\Scripts\python.exe -m app.manage create-admin admin 13800138000
.\.venv\Scripts\python.exe -m uvicorn app.main:app
```

创建管理员时由终端提示输入密码。注册接口创建借款人账号，用户名按小写存储。Linux 对应的虚拟环境解释器为 `.venv/bin/python`。

启动后打开 [Swagger UI](http://127.0.0.1:8000/docs) 或 [健康检查](http://127.0.0.1:8000/health)。

## 接口与流程

| 方法和路径 | 用途 |
| --- | --- |
| POST /api/auth/register | 提交 username、password、phone 注册 |
| POST /api/auth/login | 提交 username、password，获取 access_token |
| GET /api/users/me | 查询当前用户资料 |
| PUT /api/users/me | 设置 phone、age、income、debt 及可选 occupation |
| GET /api/loans | 查询产品和额度 |
| POST /api/applications | 提交 loanId、amount，返回 applicationId 和状态 |
| GET /api/applications/me | 查询自己的申请 |
| GET /api/applications/{application_id} | 查询申请详情 |
| POST /api/applications/{application_id}/risk-evaluation | 返回既有风控结果，或评估待风控申请 |
| GET /api/applications/{application_id}/risk-result | 查询规则评分和决策 |
| GET /api/admin/applications | 管理员查询待审核申请 |
| PUT /api/admin/applications/{application_id}/review | 管理员提交 decision，取值 approved 或 rejected |
| POST /api/applications/{application_id}/disburse | 管理员记录放款并生成计划 |
| GET /api/loans/{application_id}/repayments | 查询还款计划 |
| POST /api/repayments | 借款人提交 applicationId、amount，返回更新后的计划 |

登录后在 Swagger 的 Authorize 中填写 access_token，或发送 `Authorization: Bearer <token>`。先补全资料，再提交申请。年龄、月收入和负债取提交时的资料快照，申请与风控结果一同写入。风控结果中的 decision 保存原始自动决策，人工审批结果和还款进度以申请的 status 为准。

金额可用十进制字符串提交，响应中的金额也是十进制字符串。放款与还款为系统内部记账，计划按月拆分零利息本金，分币尾差归入最后一期。提交还款每次都会产生新的付款操作。请求超时后应先查询计划，确认付款是否已入账。

根目录的《API接口文档》记录旧版 C++ 接口。FastAPI 的请求和响应模型以本页及运行时 Swagger 为准。初始迁移面向空库，已有 C++ 数据需要单独导入。

## 数据字典

| 表 | 用途 |
| --- | --- |
| users | 账号、BCrypt 哈希、角色和借款资料 |
| loan_products | 产品、最高额度和期限（月） |
| loan_applications | 用户、产品、申请本金、风控输入快照及当前状态 |
| risk_results | 每笔申请的评分、风险等级和原始自动决策 |
| operation_logs | 操作人、动作、关联申请和时间 |
| repayment_schedules | 每期本金、到期日和已还金额 |
| repayment_transactions | 每次付款抵扣至各期的金额和时间 |

## 自动化测试

```powershell
.\.venv\Scripts\python.exe -m pytest tests -q
```

测试会用 Alembic 初始化隔离的 SQLite 数据库，覆盖认证、越权检查、三种风控决策、人工审批及还款。
