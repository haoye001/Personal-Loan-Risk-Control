from fastapi import APIRouter, HTTPException, status
from sqlalchemy.exc import IntegrityError

from app.core import create_token, hash_password, verify_password
from app.dependencies import Db
from app.models import User
from app.repositories import audit, find_user
from app.schemas.auth import LoginRequest, LoginResponse, RegisterRequest, RegisterResponse

router = APIRouter(prefix="/auth", tags=["认证"])
DUMMY_HASH = hash_password("unused-placeholder")


@router.post("/register", status_code=status.HTTP_201_CREATED)
def register(payload: RegisterRequest, db: Db) -> RegisterResponse:
    user = User(username=payload.username.lower(), password_hash=hash_password(payload.password), phone=payload.phone)
    db.add(user)
    try:
        db.flush()
        audit(db, user.id, "register")
        db.commit()
    except IntegrityError:
        db.rollback()
        if find_user(db, payload.username) is not None:
            raise HTTPException(409, "Username already exists") from None
        raise
    return RegisterResponse(success=True, message="registration successful", username=user.username)


@router.post("/login")
def login(payload: LoginRequest, db: Db) -> LoginResponse:
    user = find_user(db, payload.username)
    valid = verify_password(payload.password, user.password_hash if user else DUMMY_HASH)
    if user is None or not valid:
        raise HTTPException(401, "Invalid username or password")
    audit(db, user.id, "login")
    db.commit()
    return LoginResponse(access_token=create_token(user.id))
