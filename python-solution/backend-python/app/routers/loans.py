from fastapi import APIRouter, HTTPException
from pydantic import BaseModel
from sqlalchemy import select

from app.dependencies import Admin, CurrentUser, Db
from app.models import LoanApplication, LoanProduct, RiskResult, User
from app.repositories import applications, audit, transition
from app.schemas.loans import (
    ApplicationRequest,
    ApplicationResponse,
    ProductResponse,
    ProfileResponse,
    ProfileUpdate,
    ReviewRequest,
    RiskResponse,
)
from app.services import calculate_risk

router = APIRouter(tags=["借款业务"])
admin_router = APIRouter(prefix="/admin", tags=["管理员"])


class BorrowerSummary(BaseModel):
    id: int
    username: str


def application_response(application: LoanApplication) -> ApplicationResponse:
    return ApplicationResponse(
        applicationId=application.id,
        userId=application.user_id,
        loanId=application.loan_id,
        amount=application.amount,
        status=application.status,
    )


def get_application(db: Db, application_id: int, user: User) -> LoanApplication:
    application = db.get(LoanApplication, application_id)
    if application is None or (user.role != "admin" and application.user_id != user.id):
        raise HTTPException(404, "Application not found")
    return application


def risk_response(result: RiskResult) -> RiskResponse:
    return RiskResponse(
        applicationId=result.application_id, score=result.score, level=result.level, decision=result.decision
    )


@router.get("/users/me")
def profile(user: CurrentUser) -> ProfileResponse:
    return ProfileResponse.model_validate(user)


@router.put("/users/me")
def update_profile(payload: ProfileUpdate, user: CurrentUser, db: Db) -> ProfileResponse:
    for name, value in payload.model_dump().items():
        setattr(user, name, value)
    audit(db, user.id, "update_profile")
    db.commit()
    return ProfileResponse.model_validate(user)


@router.get("/loans")
def list_products(user: CurrentUser, db: Db) -> list[ProductResponse]:
    products = db.scalars(select(LoanProduct).order_by(LoanProduct.id))
    return [
        ProductResponse(id=product.id, name=product.name, maxAmount=product.max_amount, term=product.term)
        for product in products
    ]


@router.post("/applications", status_code=201)
def create_application(payload: ApplicationRequest, user: CurrentUser, db: Db) -> ApplicationResponse:
    product = db.get(LoanProduct, payload.loanId)
    if product is None:
        raise HTTPException(404, "Loan product not found")
    if payload.amount > product.max_amount:
        raise HTTPException(422, "Amount exceeds product limit")
    if user.age is None or user.income is None or user.debt is None:
        raise HTTPException(422, "Complete your age, income and debt profile first")
    application = LoanApplication(
        user_id=user.id, loan_id=product.id, amount=payload.amount,
        age=user.age, income=user.income, debt=user.debt,
    )
    db.add(application)
    db.flush()
    result = calculate_risk(application)
    application.status = result.decision
    db.add(result)
    audit(db, user.id, "create_application", application.id)
    audit(db, user.id, "evaluate_risk", application.id)
    db.commit()
    return application_response(application)


@router.get("/applications/me")
def my_applications(user: CurrentUser, db: Db) -> list[ApplicationResponse]:
    return [application_response(item) for item in applications(db, user_id=user.id)]


@router.get("/applications/{application_id}")
def application_detail(application_id: int, user: CurrentUser, db: Db) -> ApplicationResponse:
    return application_response(get_application(db, application_id, user))


@router.get("/applications/{application_id}/risk-result")
def get_risk(application_id: int, user: CurrentUser, db: Db) -> RiskResponse:
    get_application(db, application_id, user)
    result = db.get(RiskResult, application_id)
    if result is None:
        raise HTTPException(404, "Risk result not found")
    return risk_response(result)


@router.post("/applications/{application_id}/risk-evaluation")
def evaluate_risk(application_id: int, user: CurrentUser, db: Db) -> RiskResponse:
    application = get_application(db, application_id, user)
    result = db.get(RiskResult, application_id)
    if result is not None:
        return risk_response(result)
    result = calculate_risk(application)
    if not transition(db, application_id, "pending", result.decision):
        db.rollback()
        existing = db.get(RiskResult, application_id)
        if existing is not None:
            return risk_response(existing)
        raise HTTPException(409, "Application is not pending")
    db.add(result)
    audit(db, user.id, "evaluate_risk", application_id)
    db.commit()
    return risk_response(result)


@admin_router.get("/borrowers")
def list_borrowers(admin: Admin, db: Db) -> list[BorrowerSummary]:
    rows = db.execute(select(User.id, User.username).where(User.role == "borrower").order_by(User.id.desc()))
    return [BorrowerSummary(id=row.id, username=row.username) for row in rows]


@admin_router.get("/applications")
def pending_review(admin: Admin, db: Db) -> list[ApplicationResponse]:
    return [application_response(item) for item in applications(db, status="manual_review")]


@admin_router.put("/applications/{application_id}/review")
def review(application_id: int, payload: ReviewRequest, admin: Admin, db: Db) -> ApplicationResponse:
    application = get_application(db, application_id, admin)
    if not transition(db, application_id, "manual_review", payload.decision):
        raise HTTPException(409, "Application is not awaiting manual review")
    audit(db, admin.id, "review_" + payload.decision, application_id)
    db.commit()
    return application_response(application)
