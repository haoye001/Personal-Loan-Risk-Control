from fastapi import APIRouter, status

from app.schemas.auth import RegisterRequest, RegisterResponse

router = APIRouter()


@router.post(
    "/auth/register",
    response_model=RegisterResponse,
    status_code=status.HTTP_201_CREATED,
    summary="校验注册请求",
)
def register(payload: RegisterRequest) -> RegisterResponse:
    """第一阶段仅校验请求；第二阶段改为密码哈希后写入 MySQL。"""
    return RegisterResponse(
        success=True,
        message="registration request is valid; database persistence is not enabled yet",
        username=payload.username,
    )
