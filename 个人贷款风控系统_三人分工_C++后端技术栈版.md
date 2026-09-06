# 互联网个人贷款风控系统——三人小组分工（C++ 后端技术栈版）

## 一、项目定位

项目：多数据源融合的互联网个人贷款风控系统  
课程：进阶式挑战性综合项目 I  
方向：互联网+金融

当前阶段的目标是完成基础版本的系统设计与实现，包括：

- 可行性分析
- 需求分析
- 概要设计
- 详细设计
- 系统架构
- 模块划分
- 功能设计
- 接口设计
- 基础代码实现

系统分为两个主要端：

1. 借款人 Android APP
2. 后台审批管理 Web 系统

当前阶段**不实现真正的机器学习风控模型**，只预留风控接口，为后续阶段接入 AI 风控模型做准备。

---

# 二、总体系统架构

```text
                    互联网个人贷款风控系统
                              │
              ┌───────────────┴───────────────┐
              │                               │
       借款人 Android APP                后台审批管理 Web
              │                               │
              └───────────────┬───────────────┘
                              │
                         HTTP / JSON
                              │
                              ▼
                    ┌──────────────────┐
                    │ C++ 后端服务器    │
                    │ C++17/20         │
                    │ Drogon           │
                    └────────┬─────────┘
                             │
                    ┌────────┴─────────┐
                    │                  │
               业务 Service          DAO
                    │                  │
                    └────────┬─────────┘
                             │
                             ▼
                          MySQL
```

后续阶段：

```text
贷款申请
   ↓
多数据源
   ↓
数据清洗
   ↓
特征工程
   ↓
机器学习 / 深度学习
   ↓
风险评分
   ↓
自动 / 辅助审批
```

---

# 三、技术栈

| 模块 | 技术 |
|---|---|
| Android APP | Kotlin + Android Studio |
| 后端 | C++17/20 |
| Web 框架 | Drogon |
| API | RESTful |
| 数据交换 | JSON |
| 数据库 | MySQL |
| 数据访问 | Drogon ORM / MySQL |
| 后台管理 | Vue |
| API 测试 | Postman |
| 数据库工具 | Navicat / DataGrip |
| UML/流程图 | draw.io / StarUML |
| 原型 | Figma / Axure |
| 版本控制 | Git + GitHub/Gitee |
| 服务器环境 | Linux / Ubuntu |

---

# 四、三人分工

## 成员 A：Android APP 开发负责人

### 主要职责

负责借款人 Android APP。

主要模块：

- 用户注册
- 用户登录
- 首页
- 个人信息
- 贷款产品
- 贷款申请
- 我的贷款
- 还款记录
- 消息 / 状态通知

### APP 业务流程

```text
注册
 ↓
登录
 ↓
填写个人信息
 ↓
查看贷款产品
 ↓
提交贷款申请
 ↓
审核中
 ↓
通过 / 拒绝
 ↓
放款
 ↓
还款
 ↓
结清
```

### 学习重点

```text
Kotlin / Java 基础
 ↓
Android Studio
 ↓
Activity / Fragment
 ↓
XML / UI
 ↓
RecyclerView
 ↓
HTTP
 ↓
JSON
 ↓
OkHttp
 ↓
调用 C++ REST API
```

### 主要交付物

- APP 页面
- 页面跳转
- API 调用
- JSON 数据解析
- 登录 / 注册功能
- 贷款申请功能
- 我的贷款功能

---

# 五、成员 B：C++ 后端开发负责人

## 主要职责

负责整个后端服务器。

核心技术：

```text
C++17/20
 ↓
Drogon
 ↓
HTTP
 ↓
RESTful API
 ↓
JSON
 ↓
Service
 ↓
DAO
 ↓
MySQL
```

## 后端模块

```text
backend/
├── controller/
│   ├── UserController
│   ├── LoanController
│   ├── RepaymentController
│   ├── ApprovalController
│   └── RiskController
│
├── service/
│   ├── UserService
│   ├── LoanService
│   ├── RepaymentService
│   ├── ApprovalService
│   └── RiskService
│
├── model/
│   ├── User
│   ├── Loan
│   ├── Repayment
│   └── Approval
│
├── dao/
│   └── Database
│
├── common/
│   ├── Result
│   ├── Exception
│   └── Utils
│
└── main.cc
```

## Controller 层

负责：

```text
接收 HTTP 请求
 ↓
解析 JSON
 ↓
参数校验
 ↓
调用 Service
 ↓
返回 JSON
```

例如：

```text
POST /api/loans/apply
```

请求：

```json
{
    "userId": 10001,
    "productId": 1,
    "amount": 10000,
    "term": 12
}
```

返回：

```json
{
    "code": 200,
    "message": "申请成功",
    "data": {
        "loanId": 100001
    }
}
```

## Service 层

负责业务逻辑：

```text
贷款申请
 ↓
检查用户
 ↓
检查贷款产品
 ↓
检查申请额度
 ↓
创建贷款申请
 ↓
保存数据库
```

## DAO 层

负责：

- 数据库连接
- SQL
- 查询
- 插入
- 更新
- 删除

---

# 六、核心 API

## 用户

```text
POST /api/register
POST /api/login
GET  /api/users/{id}
PUT  /api/users/{id}
```

## 贷款

```text
GET  /api/loans
POST /api/loans/apply
GET  /api/loans/{id}
GET  /api/users/{id}/loans
```

## 还款

```text
GET  /api/repayments
POST /api/repayments
GET  /api/loans/{id}/repayments
```

## 审批

```text
GET  /api/approval/list
GET  /api/approval/{id}
POST /api/approval/{id}/approve
POST /api/approval/{id}/reject
```

## 风控

当前阶段只设计接口：

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

以后可以把：

```text
RiskService
```

替换或扩展成：

```text
C++ Backend
      ↓
RiskService
      ↓
AI Risk Model
      ↓
riskScore
```

而不需要改变 APP 和审批系统的主要接口。

---

# 七、成员 C：金融业务 + 数据库 + 后台管理 + 测试负责人

成员 C 不只是“数据库管理员”，而是负责系统的金融业务逻辑和质量保障。

## 主要职责

### 1. 金融业务

负责梳理：

```text
用户
 ↓
贷款产品
 ↓
贷款申请
 ↓
风险评估
 ↓
人工审批
 ↓
通过 / 拒绝
 ↓
放款
 ↓
还款
 ↓
结清
```

### 2. 数据库

建议核心表：

```text
user
user_profile
loan_product
loan
loan_application
approval
approval_record
repayment
repayment_record
risk_assessment
operation_log
```

### 3. E-R 图

重点设计：

```text
User
 │
 ├── UserProfile
 │
 ├── LoanApplication
 │       │
 │       └── Loan
 │              │
 │              └── Repayment
 │
 └── RiskAssessment
```

### 4. 后台管理

后台 Web 主要功能：

- 管理员登录
- 用户管理
- 贷款申请查看
- 贷款审批
- 审批记录
- 还款信息
- 数据统计

### 5. 测试

负责：

- 测试用例
- API 测试
- 数据库测试
- 功能测试
- 异常测试
- 集成测试
- 系统测试

---

# 八、三人共同负责的内容

不能把整个项目完全割裂成三块。

三个人都需要参与：

## 1. 可行性分析

比较：

```text
Java / Spring Boot
C++ / Drogon
Python / Django
Node.js
```

最终选择 C++ / Drogon 时说明：

- 与小组已有 C++ 学习基础结合
- 满足 HTTP / REST API 需求
- 能够连接 MySQL
- 能满足当前项目规模
- 后续可以扩展

同时承认：

- C++ Web 生态和开发效率相比 Java 不占优势
- 团队需要额外学习 Drogon
- 因此需要控制系统规模，避免过度工程化

## 2. 需求分析

共同确认：

- 用户需求
- 管理员需求
- 贷款业务
- 审批业务
- 还款业务
- 风控接口需求

## 3. 概要设计

共同讨论：

```text
系统架构
模块划分
技术选型
数据流
接口
安全
扩展性
```

## 4. 详细设计

共同完成：

- 类设计
- API 文档
- 数据库设计
- 页面设计
- 业务流程
- 异常处理

## 5. 最终报告与答辩

三个人都必须能够解释：

```text
为什么这样设计？
为什么选择 C++？
C++ 后端如何处理 HTTP？
APP 如何调用后端？
数据库如何设计？
风控接口如何预留？
系统未来如何接入 AI？
```

---

# 九、Git 协作

推荐：

```text
main
 │
 └── develop
      ├── feature/android
      ├── feature/backend
      └── feature/admin
```

规则：

- 不直接修改 main
- 每个人使用自己的 feature 分支
- 功能完成后合并到 develop
- 定期进行三端联调
- commit 信息清晰

例如：

```text
feat: add loan application API
feat: add Android login page
feat: add loan database tables
fix: validate loan amount
```

---

# 十、项目目录

```text
personal-loan-risk-system/
│
├── android-app/
│
├── backend/
│   ├── controller/
│   ├── service/
│   ├── model/
│   ├── dao/
│   ├── common/
│   └── main.cc
│
├── admin-web/
│
├── database/
│   ├── create.sql
│   └── test-data.sql
│
├── docs/
│   ├── 可行性分析报告
│   ├── 需求规格说明书
│   ├── 概要设计说明书
│   ├── 详细设计说明书
│   ├── API接口文档
│   ├── 数据库设计
│   └── 测试报告
│
└── README.md
```

---

# 十一、6 周项目安排

| 周次 | 成员 A | 成员 B | 成员 C | 共同输出 |
|---|---|---|---|---|
| 第1周 | APP 技术调研 | C++/Drogon 调研 | 金融业务/数据库调研 | 可行性分析 |
| 第2周 | APP 需求 | API/后端需求 | 业务/DB需求 | 需求规格说明书 |
| 第3周 | APP 架构 | 后端架构 | E-R/数据库 | 概要设计 |
| 第4周 | UI/页面详细设计 | API/Service详细设计 | DB/测试详细设计 | 详细设计 |
| 第5周 | APP实现 | C++后端实现 | 后台/DB实现 | 第一轮联调 |
| 第6周 | APP完善 | 后端完善 | 测试完善 | 集成、报告、答辩 |

---

# 十二、最终建议

本项目采用 C++ 后端是可行的，但要坚持：

> **使用成熟框架，而不是自己造 Web 框架。**

推荐路线：

```text
C++17/20
 ↓
现代 C++
 ↓
Linux
 ↓
网络基础
 ↓
Drogon
 ↓
REST API
 ↓
MySQL
 ↓
个人贷款业务
 ↓
完整后端
```

核心目标不是“证明 C++ 能做 Web”，而是：

> **利用 C++ 学习基础完成一个真正可以运行、可以联调、可以扩展的后端系统。**
