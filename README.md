# 个人贷款风控系统

本项目提供 Python / FastAPI 和 C++ / Drogon 两套方案。

| 目录 | 内容 |
| --- | --- |
| [python-solution](python-solution) | FastAPI 后端、Vue 管理端、Android 借款端和成员 SQL 示例 |
| [cpp-solution](cpp-solution) | C++ 后端、客户端副本、接口文档和设计资料 |

Python 方案支持借款人注册、登录、补充资料、提交贷款申请和查询结果。规则风控会决定是否转入人工审核。管理员可以审批申请、管理成员档案。本地联调默认使用 SQLite，服务器使用 MySQL。两种环境都通过 Alembic 管理表结构。

## 本地联调

1. 按 [FastAPI 启动说明](python-solution/backend-python/README.md) 配置 `.env`、升级数据库、初始化产品并创建管理员。
2. 在 `python-solution/admin-web` 执行 `npm ci` 和 `npm run dev`。Vite 把 `/api` 转发到本机 8000 端口。
3. 用 Android Studio 打开 `python-solution/android-app`。Android 模拟器默认连接 `http://10.0.2.2:8000`。在 App 中注册并登录，填写年龄 30、月收入 5000 元和负债 0 元，再申请默认信用贷 6000 元。用 Web 管理员账号审批，回到 App 的「我的贷款」查看状态。

## Android 安装包

打开 GitHub 的 [Android APK 工作流](https://github.com/hyG0611/Personal-Loan-Risk-Control/actions/workflows/android-apk.yml)，在成功运行的 **Artifacts** 区下载 `loan-android-debug`，解压后安装 `app-debug.apk`。这是使用调试签名的联调包，默认连接 `https://120.55.39.172`。下载并安装后，手机可直接通过公网使用。可用 **Run workflow** 指定其他后端地址重新打包。

也可以在 Android Studio 中安装项目要求的 Android SDK Platform 36 和 JDK 25，然后从 `python-solution/android-app` 执行：

```powershell
.\gradlew.bat :app:assembleDebug -PloanApiUrl=https://120.55.39.172
```

生成文件位于 `app/build/outputs/apk/debug/app-debug.apk`。

## 服务器访问

服务器通过 Nginx 提供管理端和 API。后端连接服务器上的 MySQL，数据库账号和 JWT 密钥由服务器保管。首次登录管理端前，在服务器上以管理员权限创建应用账号：

```bash
cd /opt/personal-loan-risk-control/current/backend
set -a
. /etc/personal-loan-risk-control/app.env
set +a
runuser -u loan-risk --preserve-environment -- .venv/bin/python -m app.manage create-admin YOUR_ADMIN YOUR_PHONE
unset DATABASE_URL JWT_SECRET
```

浏览器可以直接打开 [公网管理端](https://120.55.39.172/) 和 [FastAPI 文档](https://120.55.39.172/docs)。Nginx 提供 HTTPS，证书通过 systemd 定时续期。

服务器目录如下：

| 路径 | 用途 |
| --- | --- |
| `/opt/personal-loan-risk-control/current/backend/` | FastAPI、Alembic 和 Python 虚拟环境 |
| `/opt/personal-loan-risk-control/current/web/` | Vue 管理端构建文件 |
| `/opt/personal-loan-risk-control/releases/` | 部署版本，`current` 指向运行版本 |
| `/etc/personal-loan-risk-control/` | 后端和 MySQL 环境配置 |
| `/etc/nginx/conf.d/personal-loan-public.conf` | 公网 HTTPS 站点和 API 代理 |
| `/var/lib/docker/volumes/loan-risk-mysql-data/_data/` | MySQL 数据卷 |

旧版隧道 APK 的服务器地址在构建时写入。请使用公网地址重新构建的 APK。

`sql/sq1/member.sql` 是独立的 MySQL 示例。Python 管理端的成员档案保存在 Alembic 创建的 `members` 表，管理员权限来自 `users.role`。
