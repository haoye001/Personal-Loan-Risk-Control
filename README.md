# 个人贷款风控系统

本项目提供 Python / FastAPI 和 C++ / Drogon 两套方案。

| 目录 | 内容 |
| --- | --- |
| [python-solution](python-solution) | FastAPI 后端、Vue 管理端、Android 借款端和成员 SQL 示例 |
| [cpp-solution](cpp-solution) | C++ 后端、客户端副本、接口文档和设计资料 |

Python 方案支持借款人注册、登录、补充资料、提交贷款申请和查询结果。规则风控会决定是否转入人工审核。管理员可以审批申请、管理成员档案。默认使用 SQLite，数据库结构由 Alembic 管理。

## 本地联调

1. 按 [FastAPI 启动说明](python-solution/backend-python/README.md) 配置 `.env`、升级数据库、初始化产品并创建管理员。
2. 在 `python-solution/admin-web` 执行 `npm ci` 和 `npm run dev`。Vite 把 `/api` 转发到本机 8000 端口。
3. 用 Android Studio 打开 `python-solution/android-app`。Android 模拟器默认连接 `http://10.0.2.2:8000`。在 App 中注册并登录，填写年龄 30、月收入 5000 元和负债 0 元，再申请默认信用贷 6000 元。用 Web 管理员账号审批，回到 App 的「我的贷款」查看状态。

## Android 安装包

打开 GitHub 的 [Android APK 工作流](https://github.com/hyG0611/Personal-Loan-Risk-Control/actions/workflows/android-apk.yml)，在成功运行的 **Artifacts** 区下载 `loan-android-debug`，解压后安装 `app-debug.apk`。这是使用调试签名的联调包。可用 **Run workflow** 指定其他后端地址重新打包。

也可以在 Android Studio 中安装项目要求的 Android SDK Platform 37 和 JDK 25，然后从 `python-solution/android-app` 执行：

```powershell
.\gradlew.bat :app:assembleDebug -PloanApiUrl=http://127.0.0.1:18080
```

生成文件位于 `app/build/outputs/apk/debug/app-debug.apk`。

## 服务器访问

服务使用 SSH 隧道访问。用有权访问服务器的 SSH 账号执行：

```powershell
ssh -N -L 18080:127.0.0.1:18080 YOUR_USER@YOUR_SERVER
```

保持隧道打开，在浏览器访问 [管理端](http://127.0.0.1:18080/) 和 [FastAPI 文档](http://127.0.0.1:18080/docs)。使用上述默认工作流生成的 APK 时，在同一台电脑连接 Android 设备并开启 USB 调试，然后执行：

```powershell
adb reverse tcp:18080 tcp:18080
adb install -r app-debug.apk
```

`sql/sq1/member.sql` 是独立的 MySQL 示例。Python 管理端的成员档案保存在 Alembic 创建的 `members` 表，管理员权限来自 `users.role`。
