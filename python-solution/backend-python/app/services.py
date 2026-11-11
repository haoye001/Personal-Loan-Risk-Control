from calendar import monthrange
from datetime import date
from decimal import Decimal

from app.models import Installment, LoanApplication, RiskResult


def calculate_risk(application: LoanApplication) -> RiskResult:
    score = 50
    if application.income >= 10000:
        score += 30
    elif application.income >= 5000:
        score += 20
    else:
        score += 10

    if application.debt * 2 > application.income:
        score -= 20

    if application.amount > 50000:
        score -= 30
    elif application.amount > 20000:
        score -= 20
    elif application.amount > 5000:
        score -= 10

    if 22 <= application.age <= 55:
        score += 10

    score = max(0, min(100, score))
    if score >= 80:
        level, decision = "LOW", "approved"
    elif score > 60:
        level, decision = "MEDIUM", "manual_review"
    else:
        level, decision = "HIGH", "rejected"

    return RiskResult(application_id=application.id, score=score, level=level, decision=decision)


def payment_schedule(application_id: int, amount: Decimal, term: int, start: date) -> list[Installment]:
    cents, remainder = divmod(int(amount * 100), term)
    installments = []
    for number in range(1, term + 1):
        year_offset, month_index = divmod(start.month - 1 + number, 12)
        year, month = start.year + year_offset, month_index + 1
        day = min(start.day, monthrange(year, month)[1])
        principal = Decimal(cents + (remainder if number == term else 0)).scaleb(-2)
        installments.append(
            Installment(application_id=application_id, number=number, principal=principal,
                        due_date=date(year, month, day))
        )
    return installments
