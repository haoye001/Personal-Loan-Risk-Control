from datetime import date
from decimal import Decimal

from fastapi import APIRouter, HTTPException
from pydantic import BaseModel, ConfigDict, Field
from sqlalchemy import select, update

from app.dependencies import Admin, CurrentUser, Db
from app.models import Installment, LoanApplication, LoanProduct, Payment
from app.repositories import audit, transition
from app.routers.loans import application_response, get_application
from app.schemas.loans import ApplicationResponse, PositiveMoney
from app.services import payment_schedule

router = APIRouter(tags=["放款与还款"])


class PaymentRequest(BaseModel):
    model_config = ConfigDict(extra="forbid")

    applicationId: int = Field(gt=0)
    amount: PositiveMoney


class InstallmentResponse(BaseModel):
    id: int
    applicationId: int
    installmentNumber: int
    principalAmount: Decimal
    interestAmount: Decimal = Decimal("0.00")
    dueAmount: Decimal
    paidAmount: Decimal
    remainingAmount: Decimal
    dueDate: date
    status: str


def schedule_query(application_id: int):
    return select(Installment).filter_by(application_id=application_id).order_by(Installment.number)


def schedule_response(rows: list[Installment]) -> list[InstallmentResponse]:
    return [
        InstallmentResponse(
            id=row.id, applicationId=row.application_id, installmentNumber=row.number,
            principalAmount=row.principal, dueAmount=row.principal, paidAmount=row.paid,
            remainingAmount=row.principal - row.paid, dueDate=row.due_date,
            status="paid" if row.paid == row.principal else "pending",
        )
        for row in rows
    ]


@router.post("/applications/{application_id}/disburse")
def disburse(application_id: int, admin: Admin, db: Db) -> ApplicationResponse:
    application = get_application(db, application_id, admin)
    product = db.get(LoanProduct, application.loan_id)
    if not transition(db, application_id, "approved", "disbursed"):
        raise HTTPException(409, "Application is not approved")
    if product.term < 1 or application.amount * 100 < product.term:
        raise HTTPException(422, "Loan amount must allow at least one cent per installment")
    db.add_all(payment_schedule(application_id, application.amount, product.term, date.today()))
    audit(db, admin.id, "disburse", application_id)
    db.commit()
    return application_response(application)


@router.get("/loans/{application_id}/repayments")
def get_schedule(application_id: int, user: CurrentUser, db: Db) -> list[InstallmentResponse]:
    get_application(db, application_id, user)
    rows = list(db.scalars(schedule_query(application_id)))
    if not rows:
        raise HTTPException(404, "Repayment schedule not found")
    return schedule_response(rows)


@router.post("/repayments")
def repay(payload: PaymentRequest, user: CurrentUser, db: Db) -> list[InstallmentResponse]:
    application = get_application(db, payload.applicationId, user)
    if application.user_id != user.id:
        raise HTTPException(403, "Only the borrower can repay this application")
    # This write locks the application before reading or modifying its installments.
    locked = db.execute(
        update(LoanApplication)
        .where(LoanApplication.id == application.id, LoanApplication.status.in_(("disbursed", "repaying")))
        .values(status="repaying")
    )
    if locked.rowcount != 1:
        raise HTTPException(409, "Application cannot accept repayments")

    rows = list(db.scalars(schedule_query(application.id).with_for_update()))
    remaining = sum(row.principal - row.paid for row in rows)
    if payload.amount > remaining:
        raise HTTPException(422, "Amount exceeds outstanding principal")

    unapplied = payload.amount
    for row in rows:
        applied = min(unapplied, row.principal - row.paid)
        if applied:
            row.paid += applied
            db.add(Payment(installment_id=row.id, amount=applied))
            unapplied -= applied
        if not unapplied:
            break
    if payload.amount == remaining:
        application.status = "completed"
    audit(db, user.id, "repay", application.id)
    response = schedule_response(rows)
    db.commit()
    return response
