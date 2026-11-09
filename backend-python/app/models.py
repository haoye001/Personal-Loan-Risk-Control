from datetime import date, datetime
from decimal import Decimal

from sqlalchemy import Date, DateTime, ForeignKey, Numeric, String, UniqueConstraint, func
from sqlalchemy.orm import DeclarativeBase, Mapped, mapped_column


class Base(DeclarativeBase):
    pass


class User(Base):
    __tablename__ = "users"

    id: Mapped[int] = mapped_column(primary_key=True)
    username: Mapped[str] = mapped_column(String(32), unique=True)
    password_hash: Mapped[str] = mapped_column(String(60))
    phone: Mapped[str] = mapped_column(String(11))
    role: Mapped[str] = mapped_column(String(16), default="borrower")
    age: Mapped[int | None]
    income: Mapped[Decimal | None] = mapped_column(Numeric(12, 2))
    debt: Mapped[Decimal | None] = mapped_column(Numeric(12, 2))
    occupation: Mapped[str | None] = mapped_column(String(80))


class LoanProduct(Base):
    __tablename__ = "loan_products"

    id: Mapped[int] = mapped_column(primary_key=True)
    name: Mapped[str] = mapped_column(String(80))
    max_amount: Mapped[Decimal] = mapped_column(Numeric(12, 2))
    term: Mapped[int]


class LoanApplication(Base):
    __tablename__ = "loan_applications"

    id: Mapped[int] = mapped_column(primary_key=True)
    user_id: Mapped[int] = mapped_column(ForeignKey("users.id"), index=True)
    loan_id: Mapped[int] = mapped_column(ForeignKey("loan_products.id"))
    amount: Mapped[Decimal] = mapped_column(Numeric(12, 2))
    status: Mapped[str] = mapped_column(String(20), index=True, default="pending")
    age: Mapped[int]
    income: Mapped[Decimal] = mapped_column(Numeric(12, 2))
    debt: Mapped[Decimal] = mapped_column(Numeric(12, 2))


class RiskResult(Base):
    __tablename__ = "risk_results"

    application_id: Mapped[int] = mapped_column(ForeignKey("loan_applications.id"), primary_key=True)
    score: Mapped[int]
    level: Mapped[str] = mapped_column(String(8))
    decision: Mapped[str] = mapped_column(String(20))


class OperationLog(Base):
    __tablename__ = "operation_logs"

    id: Mapped[int] = mapped_column(primary_key=True)
    user_id: Mapped[int] = mapped_column(ForeignKey("users.id"))
    action: Mapped[str] = mapped_column(String(32))
    application_id: Mapped[int | None] = mapped_column(ForeignKey("loan_applications.id"))
    created_at: Mapped[datetime] = mapped_column(DateTime, server_default=func.now())


class Installment(Base):
    __tablename__ = "repayment_schedules"
    __table_args__ = (UniqueConstraint("application_id", "number"),)

    id: Mapped[int] = mapped_column(primary_key=True)
    application_id: Mapped[int] = mapped_column(ForeignKey("loan_applications.id"))
    number: Mapped[int]
    due_date: Mapped[date] = mapped_column(Date)
    principal: Mapped[Decimal] = mapped_column(Numeric(12, 2))
    paid: Mapped[Decimal] = mapped_column(Numeric(12, 2), default=Decimal("0.00"))


class Payment(Base):
    __tablename__ = "repayment_transactions"

    id: Mapped[int] = mapped_column(primary_key=True)
    installment_id: Mapped[int] = mapped_column(ForeignKey("repayment_schedules.id"), index=True)
    amount: Mapped[Decimal] = mapped_column(Numeric(12, 2))
    created_at: Mapped[datetime] = mapped_column(DateTime, server_default=func.now())
