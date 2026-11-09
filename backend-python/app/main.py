from fastapi import FastAPI

from app.routers import auth

app = FastAPI(
    title="个人贷款风控系统 API",
    version="0.1.0",
    description="第一阶段：服务骨架与用户注册请求校验。",
)

app.include_router(auth.router, prefix="/api", tags=["认证"])


@app.get("/health", tags=["系统"])
def health_check() -> dict[str, str]:
    """供部署检查和联调确认服务是否正常运行。"""
    return {"status": "ok"}
