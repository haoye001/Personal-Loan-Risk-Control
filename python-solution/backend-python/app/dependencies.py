from typing import Annotated

from fastapi import Depends, HTTPException
from fastapi.security import HTTPAuthorizationCredentials, HTTPBearer
from jwt import InvalidTokenError
from sqlalchemy.orm import Session

from app.core import token_user_id
from app.db import get_db
from app.models import User

Db = Annotated[Session, Depends(get_db)]
bearer = HTTPBearer(auto_error=False)


def current_user(
    db: Db,
    credentials: Annotated[HTTPAuthorizationCredentials | None, Depends(bearer)],
) -> User:
    error = HTTPException(401, "Invalid or missing credentials", headers={"WWW-Authenticate": "Bearer"})
    if credentials is None:
        raise error
    try:
        user_id = token_user_id(credentials.credentials)
    except (InvalidTokenError, ValueError):
        raise error from None
    user = db.get(User, user_id)
    if user is None:
        raise error
    return user


CurrentUser = Annotated[User, Depends(current_user)]


def require_admin(user: CurrentUser) -> User:
    if user.role != "admin":
        raise HTTPException(403, "Administrator role required")
    return user


Admin = Annotated[User, Depends(require_admin)]
