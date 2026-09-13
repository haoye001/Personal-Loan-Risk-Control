# 个人贷款风控系统 C++ 后端项目实战安排

> 配套文件：`C++第26天到个人贷款风控系统后端_学习路线.md`
>
> 实战起点：第43天
>
> 目标：不再以“学习知识点”为主，而是以真实项目代码为主线，把前面学习的 C++、CMake、Git、Linux、HTTP、Drogon、JSON、MySQL、Controller、Service、DAO 串成一个可以运行、调试、联调和提交的个人贷款风控系统后端。

---

# 一、实战总目标

最终形成：

```text
Android / 管理后台
        ↓
     HTTP/JSON
        ↓
   C++ / Drogon
        ↓
   Controller
        ↓
    Service
        ↓
      DAO
        ↓
     MySQL
        ↓
  贷款业务数据
```

核心业务：

```text
用户
 ↓
贷款产品
 ↓
贷款申请
 ↓
风险评估
 ↓
审批
 ↓
放款
 ↓
还款
 ↓
结清
```

第一版项目不追求复杂算法，而是优先做到：

- 后端可以启动
- API 可以访问
- 数据可以写入 MySQL
- 数据可以查询、修改
- 业务状态能够正确流转
- Controller / Service / DAO 分层清晰
- 可以用 Postman 完成接口测试
- 可以和团队其他成员进行联调
- 可以通过 Git 正常协作
- 可以独立定位常见 Bug

---

# 二、实战阶段总览

| 天数 | 实战主题 | 主要成果 |
|---|---|---|
| Day 43 | 项目启动与用户模块 | 注册、登录、用户查询 |
| Day 44 | 贷款产品模块 | 贷款产品 CRUD / 查询 |
| Day 45 | 贷款申请模块 | 用户提交贷款申请 |
| Day 46 | 贷款状态机 | 完整状态流转 |
| Day 47 | 审批模块 | 审批通过 / 拒绝 |
| Day 48 | 还款模块 | 还款计划与还款 |
| Day 49 | 风控模块 | 模拟风险评分 |
| Day 50 | 第一次完整联调 | APP → 后端 → MySQL |
| Day 51 | 统一响应与异常处理 | 统一错误格式 |
| Day 52 | 参数校验 | 非法参数拦截 |
| Day 53 | 日志系统 | INFO / WARN / ERROR |
| Day 54 | 权限与 Token 基础 | 登录态与接口保护 |
| Day 55 | 数据库优化 | 索引、事务、SQL 优化 |
| Day 56 | 项目重构 | Controller / Service / DAO 整理 |
| Day 57 | 接口测试 | 系统化测试 |
| Day 58 | Bug 修复 | 集中排错 |
| Day 59 | Git 团队协作 | 分支、合并、提交规范 |
| Day 60 | 项目验收 | 完整演示与项目总结 |

---

# 三、Day 43：项目正式启动 + 用户模块

## 今日目标

把之前的练习项目正式转换为个人贷款风控系统。

首先确认：

```text
backend/
├── CMakeLists.txt
├── include/
├── src/
├── config/
└── build/
```

建议逐步形成：

```text
src/
├── main.cpp
├── controller/
├── service/
├── dao/
├── model/
└── utils/
```

## 用户模块

实现：

```text
注册
登录
查询个人信息
修改个人信息
```

API：

```text
POST /api/register
POST /api/login
GET  /api/users/{id}
PUT  /api/users/{id}
```

## 今日重点

理解：

```text
Request
 ↓
Controller
 ↓
Service
 ↓
DAO
 ↓
MySQL
```

不要把 SQL 全部写进 Controller。

## 今日验收

能够使用 Postman：

```text
注册一个用户
 ↓
数据库出现用户记录
 ↓
查询用户
 ↓
修改用户
 ↓
再次查询
```

---

# 四、Day 44：贷款产品模块

## 今日目标

建立贷款产品数据。

例如：

```text
个人信用贷
消费贷
学生贷
```

字段可以先设计为：

```text
id
name
max_amount
term
interest_rate
status
```

API：

```text
GET  /api/loans
GET  /api/loans/{id}
POST /api/loans
PUT  /api/loans/{id}
```

第一版重点是“能够查询和管理贷款产品”。

## 今日验收

```text
数据库
 ↓
贷款产品
 ↓
Drogon
 ↓
GET /api/loans
 ↓
JSON
```

---

# 五、Day 45：贷款申请模块

## 今日目标

实现用户申请贷款。

API：

```text
POST /api/loans/apply
GET  /api/applications/{id}
GET  /api/users/{id}/applications
```

申请数据至少包含：

```text
application_id
user_id
loan_id
amount
term
status
created_at
```

流程：

```text
用户
 ↓
提交申请
 ↓
Controller
 ↓
LoanService
 ↓
LoanDAO
 ↓
MySQL
```

默认状态：

```text
PENDING
```

## 今日重点

不要只关注“接口能不能返回”。

要开始思考：

```text
数据是谁创建的？
数据保存在哪里？
状态什么时候改变？
谁有权限修改？
```

---

# 六、Day 46：贷款状态机

## 今日目标

让贷款申请真正拥有业务流程。

状态：

```text
PENDING
    ↓
REVIEWING
    ↓
APPROVED / REJECTED
    ↓
DISBURSED
    ↓
REPAYING
    ↓
COMPLETED
```

## 必须限制非法状态跳转

例如：

```text
PENDING → REVIEWING     合法
REVIEWING → APPROVED    合法
REVIEWING → REJECTED    合法
APPROVED → DISBURSED    合法
COMPLETED → APPROVED    非法
REJECTED → REPAYING     非法
```

可以在 Service 层实现：

```cpp
bool canTransition(Status from, Status to);
```

## 今日重点

第一次真正编写“业务规则”。

---

# 七、Day 47：审批模块

## 今日目标

实现后台审批。

API：

```text
GET  /api/approval/list
POST /api/approval/{id}/approve
POST /api/approval/{id}/reject
```

流程：

```text
贷款申请
 ↓
审核
 ├── 通过
 │    ↓
 │  APPROVED
 │
 └── 拒绝
      ↓
    REJECTED
```

## 今日重点

审批不是简单地：

```cpp
status = APPROVED;
```

而应该检查：

```text
申请是否存在？
当前状态是否允许审批？
审批人是否合法？
是否已经审批？
```

这些检查放在 Service 层。

---

# 八、Day 48：还款模块

## 今日目标

实现基础还款业务。

API：

```text
GET  /api/loans/{id}/repayments
POST /api/repayments
```

基本流程：

```text
贷款
 ↓
生成还款计划
 ↓
产生应还金额
 ↓
用户还款
 ↓
更新剩余金额
 ↓
全部还清
 ↓
COMPLETED
```

第一版不需要复杂金融计算。

可以先支持：

```text
本金
利息
应还金额
已还金额
剩余金额
还款状态
```

---

# 九、Day 49：风控模块

## 今日目标

建立风控接口。

API：

```text
POST /api/risk/evaluate
```

请求：

```json
{
    "userId": 10001,
    "loanId": 100001
}
```

返回：

```json
{
    "riskScore": 75,
    "riskLevel": "LOW",
    "result": "PASS"
}
```

## 第一版暂时不接 AI

使用简单模拟规则：

```text
收入较高       → 加分
负债较低       → 加分
历史逾期       → 减分
贷款金额过高   → 减分
```

例如：

```text
基础分 60

收入稳定 +10
无逾期 +10
金额合理 +5

最终 85
```

## 今日重点

先把：

```text
风控接口
 ↓
业务数据
 ↓
风险评分
 ↓
JSON
```

跑通。

AI 模型以后再接。

---

# 十、Day 50：第一次完整联调

## 今日目标

把整个业务链路串起来。

完整流程：

```text
注册
 ↓
登录
 ↓
查看贷款产品
 ↓
提交贷款申请
 ↓
风险评估
 ↓
审批
 ↓
放款
 ↓
生成还款计划
 ↓
还款
 ↓
结清
```

系统：

```text
Android / Postman
        ↓
HTTP
        ↓
Drogon
        ↓
Controller
        ↓
Service
        ↓
DAO
        ↓
MySQL
```

## 今日验收

至少完成一次完整业务流程。

这是第一阶段最重要的里程碑。

---

# 十一、Day 51：统一响应与异常处理

## 今日目标

统一 API 返回格式。

成功：

```json
{
    "code": 200,
    "message": "success",
    "data": {}
}
```

失败：

```json
{
    "code": 400,
    "message": "贷款金额不合法",
    "data": null
}
```

需要处理：

```text
参数错误
用户不存在
贷款不存在
申请不存在
重复申请
金额非法
状态非法
数据库异常
```

## 今日重点

让不同 Controller 的返回风格统一。

---

# 十二、Day 52：参数校验

## 今日目标

开始系统检查用户输入。

例如贷款金额：

```text
amount <= 0
```

直接拒绝。

贷款期限：

```text
term <= 0
```

直接拒绝。

用户 ID：

```text
id 不存在
```

返回错误。

## 思考方式

以后任何 API 都先问：

```text
输入是否合法？
 ↓
资源是否存在？
 ↓
当前状态是否合法？
 ↓
执行操作
```

---

# 十三、Day 53：日志系统

## 今日目标

加入基本日志。

至少区分：

```text
INFO
WARN
ERROR
```

记录：

```text
用户登录
贷款申请
审批
还款
风控
数据库错误
异常请求
```

示例：

```text
[INFO] user login: 10001
[INFO] loan application: 20001
[WARN] invalid loan amount
[ERROR] database connection failed
```

## 今日重点

学会通过日志定位：

```text
请求到了哪里？
哪一步失败？
失败原因是什么？
```

---

# 十四、Day 54：Token 与基础权限

## 今日目标

理解：

```text
登录
 ↓
Token
 ↓
客户端保存 Token
 ↓
后续请求携带 Token
 ↓
后端验证
```

第一版可以先做简单 Token 机制。

同时区分：

```text
普通用户
管理员 / 审批人员
```

例如：

```text
普通用户：
查看贷款
提交申请
查看自己的贷款
还款

管理员：
查看审批列表
审批贷款
```

---

# 十五、Day 55：数据库优化

## 今日目标

开始理解数据库性能。

检查：

```text
主键
外键
索引
JOIN
事务
```

重点关注经常查询的字段：

```text
user_id
loan_id
application_id
status
```

## 事务场景

例如审批：

```text
更新申请状态
+
记录审批信息
```

应该考虑：

```text
要么全部成功
要么全部失败
```

这就是事务。

---

# 十六、Day 56：项目重构

## 今日目标

检查整个项目结构。

推荐逐步形成：

```text
backend/
├── CMakeLists.txt
├── config/
├── include/
│   ├── controller/
│   ├── service/
│   ├── dao/
│   ├── model/
│   └── utils/
├── src/
│   ├── controller/
│   ├── service/
│   ├── dao/
│   ├── model/
│   └── utils/
└── tests/
```

检查：

```text
Controller 有没有写 SQL？
Service 有没有处理 HTTP 细节？
DAO 有没有写业务规则？
重复代码是否过多？
命名是否统一？
```

目标：

```text
Controller
    ↓
Service
    ↓
DAO
```

职责清晰。

---

# 十七、Day 57：接口测试

## 今日目标

使用 Postman 或其他 HTTP 工具进行系统测试。

建立测试清单：

```text
用户
├── 注册
├── 登录
├── 查询
└── 修改

贷款
├── 产品查询
├── 产品详情
└── 申请贷款

审批
├── 审批列表
├── 通过
└── 拒绝

还款
├── 查询计划
└── 还款

风控
└── 风险评估
```

同时测试异常：

```text
不存在的用户
不存在的贷款
负数金额
重复申请
非法状态
错误 Token
```

---

# 十八、Day 58：集中 Bug 修复

## 今日目标

不要继续增加新功能。

专门修 Bug。

建立：

```text
Bug
 ↓
复现
 ↓
定位
 ↓
修改
 ↓
重新测试
 ↓
确认修复
```

常见问题：

```text
HTTP 404
HTTP 400
HTTP 500
JSON 字段错误
SQL 错误
数据库连接错误
空指针
状态流转错误
```

## 今日原则

不要看到 Bug 就直接改一行代码。

先回答：

```text
Bug 在哪一层？
Controller？
Service？
DAO？
数据库？
```

---

# 十九、Day 59：Git 团队协作

## 今日目标

正式进入团队开发方式。

推荐：

```text
main
  │
develop
  │
  ├── feature/user
  ├── feature/loan
  ├── feature/approval
  └── feature/repayment
```

每个人：

```text
拉取 develop
 ↓
创建自己的 feature 分支
 ↓
开发
 ↓
commit
 ↓
push
 ↓
合并到 develop
```

## Commit 示例

```text
feat: add user register api
feat: add loan application
fix: validate loan amount
refactor: split loan service
test: add repayment api tests
```

## 今日重点

避免：

```text
所有人直接修改 main
```

---

# 二十、Day 60：项目最终验收

## 最终完整演示

演示：

```text
1. 注册用户
      ↓
2. 登录
      ↓
3. 查看贷款产品
      ↓
4. 提交贷款申请
      ↓
5. 风险评估
      ↓
6. 管理员审批
      ↓
7. 放款
      ↓
8. 查看还款计划
      ↓
9. 进行还款
      ↓
10. 最终结清
```

## 最终架构

```text
                    Client
                      │
                 HTTP / JSON
                      │
                  Drogon
                      │
              ┌───────┴───────┐
              │  Controller   │
              └───────┬───────┘
                      │
              ┌───────▼───────┐
              │    Service    │
              └───────┬───────┘
                      │
              ┌───────▼───────┐
              │      DAO      │
              └───────┬───────┘
                      │
              ┌───────▼───────┐
              │     MySQL     │
              └───────────────┘
```

业务：

```text
User
 │
 ├── Loan Product
 │
 └── Loan Application
          │
          ├── Risk
          │
          ├── Approval
          │
          └── Repayment
```

---

# 二十一、每一天的固定实战流程

以后每天学习不再采用单纯的“讲知识点 → 做题”。

统一采用：

```text
① 回顾昨天代码
        ↓
② 明确今天功能
        ↓
③ 设计数据结构
        ↓
④ 设计 API
        ↓
⑤ 写 Controller
        ↓
⑥ 写 Service
        ↓
⑦ 写 DAO
        ↓
⑧ 连接 MySQL
        ↓
⑨ Postman 测试
        ↓
⑩ 修 Bug
        ↓
⑪ Git commit
        ↓
⑫ 总结
```

---

# 二十二、每个模块的完成标准

## 用户模块

```text
[ ] 注册
[ ] 登录
[ ] 查询用户
[ ] 修改用户
[ ] 数据进入 MySQL
```

## 贷款模块

```text
[ ] 查询贷款产品
[ ] 查看详情
[ ] 创建贷款产品
[ ] 修改贷款产品
```

## 申请模块

```text
[ ] 提交申请
[ ] 查询申请
[ ] 查询用户申请
[ ] 防止重复申请
```

## 审批模块

```text
[ ] 审批列表
[ ] 审批通过
[ ] 审批拒绝
[ ] 状态检查
```

## 还款模块

```text
[ ] 生成计划
[ ] 查询计划
[ ] 还款
[ ] 更新剩余金额
[ ] 完成结清
```

## 风控模块

```text
[ ] 风险评估接口
[ ] 风险评分
[ ] 风险等级
[ ] PASS / REJECT
```

---

# 二十三、第一版项目暂时不要做什么

为了避免项目失控，第一版暂时不加入：

```text
复杂 AI 模型
Redis
消息队列
微服务
Kubernetes
复杂分布式系统
复杂金融计息
高并发优化
复杂推荐算法
```

先做到：

```text
能运行
+
能存数据
+
能调用 API
+
业务流程正确
+
代码结构清晰
```

之后再逐步升级。

---

# 二十四、第二阶段升级路线

第一版完成以后：

```text
V1
基础贷款业务
 ↓
V2
异常处理 + 日志 + Token
 ↓
V3
Redis + 缓存
 ↓
V4
性能优化 + 连接池
 ↓
V5
真实风控模型
 ↓
V6
AI / 机器学习风险预测
```

最终目标：

```text
C++
 ↓
Drogon
 ↓
REST API
 ↓
MySQL
 ↓
Redis
 ↓
风控规则
 ↓
机器学习模型
 ↓
完整互联网贷款风控后端
```

---

# 二十五、项目实战最重要的原则

## 原则 1：先跑通，再优化

第一版允许代码不完美。

优先：

```text
功能正确
```

再考虑：

```text
代码漂亮
性能优秀
架构复杂
```

## 原则 2：每个功能都必须真正落到代码

不能只说：

```text
今天学习贷款申请。
```

而要完成：

```text
API
+
Controller
+
Service
+
DAO
+
SQL
+
测试
```

## 原则 3：遇到 Bug 要自己定位

优先按照：

```text
请求
 ↓
Controller
 ↓
Service
 ↓
DAO
 ↓
MySQL
```

逐层排查。

## 原则 4：不要为了复杂而复杂

项目目前是学习型真实项目。

核心目标：

> **通过一个真实业务系统，把 C++ 从“会写代码”提升到“会做后端项目”。**

---

# 二十六、最终项目成果清单

完成 Day 60 后，至少应该拥有：

```text
[ ] C++17 后端项目
[ ] CMake 构建
[ ] Git 仓库
[ ] Drogon Web 服务
[ ] REST API
[ ] JSON
[ ] MySQL
[ ] Controller
[ ] Service
[ ] DAO
[ ] 用户模块
[ ] 贷款模块
[ ] 申请模块
[ ] 审批模块
[ ] 还款模块
[ ] 风控模块
[ ] 日志
[ ] 异常处理
[ ] 基础 Token
[ ] Postman 测试
[ ] 团队 Git 协作
[ ] 完整业务联调
```

最终验收标准：

> **能够独立解释项目架构，能够启动后端，能够调用主要 API，能够追踪一次请求从 Controller 到 Service、DAO 再到 MySQL 的完整路径，并能够自己定位和修复常见问题。**

这份实战安排与学习路线配合使用：

```text
学习路线
    ↓
学习知识
    ↓
项目实战安排
    ↓
写真实代码
    ↓
测试
    ↓
Git
    ↓
联调
    ↓
项目完成
```
