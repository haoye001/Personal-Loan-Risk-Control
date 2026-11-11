from sqlalchemy import select, update
from sqlalchemy.orm import Session

from app.models import LoanApplication, OperationLog, User


def find_user(db: Session, username: str) -> User | None:
    return db.scalar(select(User).where(User.username == username.lower()))


def applications(db: Session, **filters) -> list[LoanApplication]:
    query = select(LoanApplication).filter_by(**filters).order_by(LoanApplication.id.desc())
    return list(db.scalars(query))


def transition(db: Session, application_id: int, expected: str, target: str) -> bool:
    result = db.execute(
        update(LoanApplication)
        .where(LoanApplication.id == application_id, LoanApplication.status == expected)
        .values(status=target)
    )
    return result.rowcount == 1


def audit(db: Session, user_id: int, action: str, application_id: int | None = None) -> None:
    db.add(OperationLog(user_id=user_id, action=action, application_id=application_id))
