# 个人贷款风控系统接口文档

## 1. 基本约定

- **服务地址**：`http://<服务器地址>:8888`
- **接口前缀**：`/api`
- **数据格式**：请求体与响应体均为 JSON；请求时请设置 `Content-Type: application/json`。
- **鉴权**：当前后端尚未实现 Token、Cookie 或其他鉴权机制。前端登录后需自行保存用户 `id`，并在需要 `userId` 的接口中传入。
- **金额单位**：元。

除非特别说明，响应均含有 `success` 字段。调用方应同时判断 HTTP 状态码和 `success`，不能只依据 HTTP 200 判断业务成功。

## 2. 接口一览

| 模块 | 方法 | 路径 | 用途 |
| --- | --- | --- | --- |
| 用户 | POST | `/api/register` | 注册 |
| 用户 | POST | `/api/login` | 登录 |
| 用户 | GET | `/api/users/{id}` | 查询用户资料 |
| 用户 | PUT | `/api/users/{id}` | 更新用户资料 |
| 贷款产品 | GET | `/api/loans` | 获取贷款产品列表 |
| 贷款申请 | POST | `/api/apply` | 提交贷款申请 |
| 贷款申请 | GET | `/api/applications/{userId}` | 查询用户申请记录 |
| 风控 | POST | `/api/risk/check` | 执行风控评估 |
| 风控 | GET | `/api/risk/result/{applicationId}` | 查询最新风控结果 |
| 人工审核 | PUT | `/api/applications/review` | 人工审批申请 |

## 3. 用户接口

### 3.1 注册

`POST /api/register`

请求体：

```json
{
  "username": "zhangsan",
  "password": "123456",
  "phone": "13800138000"
}
```

| 字段 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| username | string | 是 | 用户名，应保持唯一 |
| password | string | 是 | 密码 |
| phone | string | 是 | 手机号 |

成功响应（200）：

```json
{ "success": true, "message": "register success" }
```

失败响应：400（JSON 格式不合法）或 409（用户名已存在）。

```json
{ "success": false, "message": "username already exists" }
```

### 3.2 登录

`POST /api/login`

请求体：

```json
{ "username": "zhangsan", "password": "123456" }
```

成功响应（200）：

```json
{ "success": true, "message": "login success" }
```

失败响应：400（JSON 格式不合法）或 401（用户名或密码错误）。当前登录接口**不返回用户 ID，也不签发 Token**；若前端需要根据登录结果跳转或保存用户身份，需后端补充该能力。

### 3.3 查询用户资料

`GET /api/users/{id}`

路径参数：`id` 为用户 ID。

成功响应（200）：

```json
{
  "success": true,
  "id": 1,
  "username": "zhangsan",
  "phone": "13800138000"
}
```

失败响应：404。

```json
{ "success": false, "message": "user not found" }
```

> 注意：该接口仅返回 ID、用户名和手机号；年龄、收入、职业、负债及密码均不返回。

### 3.4 更新用户资料

`PUT /api/users/{id}`

路径参数：`id` 为用户 ID。

请求体：

```json
{
  "username": "zhangsan_new",
  "password": "123456",
  "phone": "13900139000"
}
```

| 字段 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| username | string | 是 | 更新后的用户名 |
| password | string | 是 | 当前实现会读取，但**不会写入数据库** |
| phone | string | 是 | 更新后的手机号 |

成功响应（200）：

```json
{ "success": true, "message": "user update success" }
```

失败响应：400。

```json
{ "success": false, "message": "user update failed" }
```

## 4. 贷款产品接口

### 4.1 获取贷款产品列表

`GET /api/loans`

成功响应（200）：

```json
{
  "success": true,
  "loans": [
    { "id": 1, "name": "信用贷", "maxAmount": 50000.0, "term": 12 }
  ]
}
```

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| id | integer | 贷款产品 ID |
| name | string | 产品名称 |
| maxAmount | number | 最大可贷金额（元） |
| term | integer | 期限；具体单位以数据库产品配置为准 |

服务端读取产品失败时返回 500：`{ "success": false, "message": "failed to get loan products" }`。

## 5. 贷款申请接口

### 5.1 提交贷款申请

`POST /api/apply`

请求体：

```json
{ "userId": 1, "loanId": 2, "amount": 10000 }
```

| 字段 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| userId | integer | 是 | 申请用户 ID |
| loanId | integer | 是 | 贷款产品 ID |
| amount | number | 是 | 申请金额（元） |

成功响应（200）：

```json
{ "success": true, "message": "apply success" }
```

失败响应（当前实现均为 200）：

```json
{ "success": false, "message": "apply failed" }
```

JSON 格式不合法时为：`{ "success": false, "message": "invalid json" }`。

> 此接口不返回新建申请的 `applicationId`，且当前控制器未校验用户、产品、金额上限或金额正数。前端应先按产品 `maxAmount` 做校验；若后续需立即发起风控，建议后端补充申请 ID 返回值。

### 5.2 查询某用户的申请记录

`GET /api/applications/{userId}`

路径参数：`userId` 为用户 ID。

成功响应（200）：

```json
{
  "success": true,
  "applications": [
    {
      "id": 12,
      "loanId": 2,
      "amount": 10000.0,
      "status": "manual_review",
      "riskScore": 70,
      "riskLevel": "MEDIUM",
      "decision": "manual_review"
    }
  ]
}
```

申请按 ID 倒序排列。尚未完成风控的申请中，`riskScore` 为 `0`，`riskLevel` 与 `decision` 为空字符串。

查询失败时当前实现返回 200：`{ "success": false, "message": "query failed" }`。

## 6. 风控接口

### 6.1 执行风控评估

`POST /api/risk/check`

请求体：

```json
{
  "userId": 1,
  "applicationId": 12,
  "loanId": 2,
  "amount": 10000
}
```

| 字段 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| userId | integer | 是 | 申请所属用户 ID |
| applicationId | integer | 是 | 待评估申请 ID |
| loanId | integer | 是 | 贷款产品 ID |
| amount | number | 是 | 申请金额（必须大于 0） |

成功响应（200）：

```json
{
  "success": true,
  "applicationId": 12,
  "score": 70,
  "level": "MEDIUM",
  "decision": "manual_review"
}
```

字段取值：

| 字段 | 可能值 | 说明 |
| --- | --- | --- |
| score | 0–100 | 风险评分，分数越高风险越低 |
| level | `LOW` / `MEDIUM` / `HIGH` | 风险等级 |
| decision | `approved` / `manual_review` / `rejected` | 自动决策 |

评估完成后，申请状态会同步更新为对应的 `approved`、`manual_review` 或 `rejected`。同一申请可重复调用，系统会保留多条风控记录；查询接口返回最新一条。

主要失败响应：

| HTTP 状态 | message | 场景 |
| --- | --- | --- |
| 400 | `invalid json` | JSON 格式不合法（当前实现未显式设置状态码，实际可能为 200） |
| 400 | `missing required parameters` | 缺少任一必填字段 |
| 400 | `invalid parameter values` | 任一 ID 或金额不大于 0 |
| 404 | `application not found` | 申请不存在 |
| 403 | `application does not belong to user` | 申请不属于所传用户 |
| 200 | `user not found` | 用户不存在（当前实现未显式设置状态码） |
| 200 | `save risk result failed` | 风控结果保存失败（当前实现未显式设置状态码） |
| 200 | `risk saved, but update application status failed` | 风控已保存但申请状态同步失败（当前实现未显式设置状态码） |

### 6.2 查询风控结果

`GET /api/risk/result/{applicationId}`

路径参数：`applicationId` 为申请 ID。

成功响应（200）：

```json
{
  "success": true,
  "applicationId": 12,
  "score": 70,
  "level": "MEDIUM",
  "decision": "manual_review"
}
```

不存在风控结果时返回 404：`{ "success": false, "message": "risk result not found" }`。

## 7. 人工审核接口

### 7.1 审核申请

`PUT /api/applications/review`

请求体：

```json
{ "applicationId": 12, "decision": "approved" }
```

| 字段 | 类型 | 必填 | 可选值 / 说明 |
| --- | --- | --- | --- |
| applicationId | integer | 是 | 大于 0 的申请 ID |
| decision | string | 是 | `approved` 或 `rejected` |

成功响应（200）：

```json
{ "success": true, "applicationId": 12, "status": "approved" }
```

失败响应：400（缺少字段或参数非法）或 500（更新申请状态或最新风控记录失败）。

```json
{ "success": false, "message": "invalid applicationId or decision" }
```

> 人工审核会同时更新申请表中的 `status` 和该申请最新一条风控结果的 `decision`。应只对 `manual_review` 状态的申请展示审核入口；后端当前未强制校验该前置状态。

## 8. 建议的前端调用流程

1. 注册或登录；登录成功后保存业务侧已知的用户 ID。
2. 调用 `GET /api/loans` 获取产品，并校验申请金额不超过 `maxAmount`。
3. 调用 `POST /api/apply` 创建申请。
4. 从 `GET /api/applications/{userId}` 获取最新申请 ID，再调用 `POST /api/risk/check` 进行评估。
5. 根据 `decision` 展示“通过”“拒绝”或“进入人工审核”。
6. 对人工审核结果调用 `PUT /api/applications/review`，最后刷新申请列表。

## 9. 前后端联调注意事项

- `POST /api/apply` 未返回 `applicationId`，因此要执行后续风控只能重新查询申请列表并取目标记录；并发提交时可能取错记录。
- 登录不返回 `userId`；登录后若前端尚未持有用户 ID，无法可靠进入后续业务流程。
- 用户更新接口仅实际更新 `username`、`phone`，密码字段不会更新；用户画像字段（年龄、收入、职业、负债）暂没有对外维护接口，但它们会影响风控评分。
- 若前端运行在不同域名或端口，后端还需要配置 CORS，否则浏览器会拦截跨域请求。
