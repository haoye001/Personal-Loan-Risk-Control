from decimal import Decimal

import pytest
from alembic import command
from alembic.config import Config
from fastapi.testclient import TestClient
from sqlalchemy import create_engine, event, func, select
from sqlalchemy.orm import Session
from sqlalchemy.pool import StaticPool

from app import core, db
from app.core import hash_password
from app.main import app
from app.models import LoanProduct, Payment, RiskResult, User


@pytest.fixture
def client(monkeypatch):
    engine = create_engine(
        "sqlite://", poolclass=StaticPool, connect_args={"check_same_thread": False}
    )

    @event.listens_for(engine, "connect")
    def enable_foreign_keys(connection, _):
        connection.execute("PRAGMA foreign_keys = ON")

    monkeypatch.setattr(db, "engine", engine)
    monkeypatch.setattr(core, "JWT_SECRET", "test-only-secret-" * 3)
    command.upgrade(Config("alembic.ini"), "head")
    with Session(engine) as session:
        session.add(LoanProduct(id=1, name="Credit", max_amount=Decimal("50000"), term=3))
        session.add(User(username="admin", phone="13800138000",
                         password_hash=hash_password("Admin1234"), role="admin"))
        session.commit()
    with TestClient(app) as test_client:
        yield test_client
    engine.dispose()


def login(client, username, password):
    response = client.post("/api/auth/login", json={"username": username, "password": password})
    assert response.status_code == 200, response.text
    return {"Authorization": "Bearer " + response.json()["access_token"]}


def borrower(client, username, income=5000, debt=0):
    payload = {"username": username, "password": "Borrower123", "phone": "13800138001"}
    assert client.post("/api/auth/register", json=payload).status_code == 201
    assert client.post("/api/auth/register", json=payload).status_code == 409
    headers = login(client, username, payload["password"])
    response = client.put("/api/users/me", headers=headers, json={
        "age": 30, "income": income, "debt": debt, "phone": payload["phone"],
    })
    assert response.status_code == 200, response.text
    return headers


def test_authentication_and_validation(client):
    assert client.get("/health").json() == {"status": "ok"}
    assert client.get("/openapi.json").status_code == 200
    assert client.get("/api/loans").status_code == 401
    assert client.get("/api/loans", headers={"Authorization": "Bearer invalid"}).status_code == 401
    secret = "invalidpassword"
    response = client.post("/api/auth/register", json={
        "username": "someone", "password": secret, "phone": "13800138000",
    })
    assert response.status_code == 422
    assert secret not in response.text
    assert client.post("/api/auth/login", json={"username": "admin", "password": "Wrong123"}).status_code == 401


def test_manual_review_and_repayment(client):
    owner = borrower(client, "owner")
    stranger = borrower(client, "stranger")
    admin = login(client, "admin", "Admin1234")
    for amount in ["0", "-1", "1.001", "50000.01", "10000000000.00"]:
        response = client.post("/api/applications", headers=owner, json={"loanId": 1, "amount": amount})
        assert response.status_code == 422, response.text
    response = client.post("/api/applications", headers=owner, json={"loanId": 1, "amount": "6000.01"})
    assert response.status_code == 201, response.text
    data = response.json()
    assert data["status"] == "manual_review"
    application_id = data["applicationId"]
    application = f"/api/applications/{application_id}"
    review = f"/api/admin/applications/{application_id}/review"
    schedule = f"/api/loans/{application_id}/repayments"
    assert client.get(application, headers=stranger).status_code == 404
    assert client.get(application + "/risk-result", headers=stranger).status_code == 404
    assert client.get(schedule, headers=stranger).status_code == 404
    assert client.put(review, headers=owner, json={"decision": "approved"}).status_code == 403
    assert client.get("/api/admin/applications", headers=admin).json() == [data]
    risk = client.get(application + "/risk-result", headers=owner).json()
    assert risk["score"] == 70
    assert client.post(application + "/risk-evaluation", headers=owner).json() == risk
    assert client.put(review, headers=admin, json={"decision": "approved"}).json()["status"] == "approved"
    assert client.put(review, headers=admin, json={"decision": "rejected"}).status_code == 409
    assert client.post(application + "/disburse", headers=owner).status_code == 403
    assert client.post(application + "/disburse", headers=admin).json()["status"] == "disbursed"
    assert client.post(application + "/disburse", headers=admin).status_code == 409
    plan = client.get(schedule, headers=owner).json()
    assert [Decimal(row["dueAmount"]) for row in plan] == list(map(Decimal, ["2000", "2000", "2000.01"]))

    def pay(amount):
        return client.post("/api/repayments", headers=owner, json={
            "applicationId": application_id, "amount": amount,
        })

    assert pay("6000.02").status_code == 422
    assert client.get(application, headers=owner).json()["status"] == "disbursed"
    assert pay("2500").status_code == 200
    assert client.get(application, headers=owner).json()["status"] == "repaying"
    result = pay("3500.01")
    assert result.status_code == 200, result.text
    assert all(row["status"] == "paid" for row in result.json())
    assert client.get(application, headers=owner).json()["status"] == "completed"
    assert pay("1").status_code == 409
    with Session(db.engine) as session:
        assert session.scalar(select(func.count()).select_from(RiskResult)) == 1
        assert session.scalar(select(func.sum(Payment.amount))) == Decimal("6000.01")


@pytest.mark.parametrize(("income", "debt", "status"), [(10000, 0, "approved"), (3000, 2000, "rejected")])
def test_automatic_decisions(client, income, debt, status):
    headers = borrower(client, "auto", income, debt)
    response = client.post("/api/applications", headers=headers, json={"loanId": 1, "amount": "3000"})
    assert response.status_code == 201, response.text
    assert response.json()["status"] == status
    assert client.get("/api/applications/me", headers=headers).json() == [response.json()]
