from decimal import Decimal
from typing import Annotated, Literal

from pydantic import BaseModel, ConfigDict, Field

from app.schemas.auth import Phone

Money = Annotated[Decimal, Field(ge=0, max_digits=12, decimal_places=2)]
PositiveMoney = Annotated[Money, Field(gt=0)]
Age = Annotated[int, Field(ge=18, le=120)]


class ProfileUpdate(BaseModel):
    model_config = ConfigDict(extra="forbid")

    phone: Phone
    age: Age
    income: Money
    debt: Money
    occupation: Annotated[str, Field(min_length=1, max_length=80)] | None = None


class ProfileResponse(BaseModel):
    model_config = ConfigDict(from_attributes=True)

    id: int
    username: str
    phone: str
    role: str
    age: int | None
    income: Decimal | None
    debt: Decimal | None
    occupation: str | None


class ApplicationRequest(BaseModel):
    model_config = ConfigDict(extra="forbid")

    loanId: Annotated[int, Field(gt=0)]
    amount: PositiveMoney


class ApplicationResponse(BaseModel):
    applicationId: int
    userId: int
    loanId: int
    amount: Decimal
    status: str


class ProductResponse(BaseModel):
    id: int
    name: str
    maxAmount: Decimal
    term: int


class RiskResponse(BaseModel):
    applicationId: int
    score: int
    level: str
    decision: str


class ReviewRequest(BaseModel):
    model_config = ConfigDict(extra="forbid")

    decision: Literal["approved", "rejected"]
