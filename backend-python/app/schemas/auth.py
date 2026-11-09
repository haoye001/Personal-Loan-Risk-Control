from typing import Annotated

from pydantic import AfterValidator, BaseModel, Field, field_validator

Username = Annotated[
    str,
    Field(
        min_length=3,
        max_length=32,
        pattern=r"^[A-Za-z0-9_]+$",
        description="3 至 32 位，仅允许字母、数字和下划线。",
    ),
]
def validate_password_bytes(value: str) -> str:
    if len(value.encode()) > 72:
        raise ValueError("password must be at most 72 UTF-8 bytes")
    return value


Password = Annotated[
    str,
    Field(min_length=8, max_length=72, description="至少 8 字符，最多 72 UTF-8 字节。"),
    AfterValidator(validate_password_bytes),
]
Phone = Annotated[
    str,
    Field(pattern=r"^1[3-9]\d{9}$", description="中国大陆 11 位手机号。"),
]


class RegisterRequest(BaseModel):
    username: Username
    password: Password
    phone: Phone

    @field_validator("password")
    @classmethod
    def password_must_contain_letter_and_number(cls, value: str) -> str:
        if not any(char.isalpha() for char in value) or not any(char.isdigit() for char in value):
            raise ValueError("password must contain at least one letter and one number")
        return value


class RegisterResponse(BaseModel):
    success: bool
    message: str
    username: str


class LoginRequest(BaseModel):
    username: Username
    password: Password


class LoginResponse(BaseModel):
    access_token: str
    token_type: str = "bearer"
