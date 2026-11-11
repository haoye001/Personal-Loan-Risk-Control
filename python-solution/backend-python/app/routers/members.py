from typing import Annotated

from fastapi import APIRouter, HTTPException, Response
from pydantic import BaseModel, ConfigDict, StringConstraints
from sqlalchemy import select
from sqlalchemy.exc import IntegrityError

from app.dependencies import Admin, Db
from app.models import Member

router = APIRouter(prefix="/admin/members", tags=["成员管理"])
Label = Annotated[str, StringConstraints(strip_whitespace=True, min_length=1, max_length=50)]
Phone = Annotated[str, StringConstraints(pattern=r"^1[3-9]\d{9}$")]


class MemberInput(BaseModel):
    model_config = ConfigDict(extra="forbid")

    name: Label
    account: Label
    phone: Phone
    role: Annotated[str, StringConstraints(strip_whitespace=True, min_length=1, max_length=30)]


class MemberOutput(MemberInput):
    model_config = ConfigDict(from_attributes=True)

    id: int


def get_member(db: Db, member_id: int) -> Member:
    member = db.get(Member, member_id)
    if member is None:
        raise HTTPException(404, "Member not found")
    return member


def save(db: Db, member: Member) -> MemberOutput:
    try:
        db.commit()
    except IntegrityError:
        db.rollback()
        raise HTTPException(409, "Member account already exists") from None
    return MemberOutput.model_validate(member)


@router.get("")
def list_members(admin: Admin, db: Db) -> list[MemberOutput]:
    return [MemberOutput.model_validate(row) for row in db.scalars(select(Member).order_by(Member.id))]


@router.post("", status_code=201)
def create_member(payload: MemberInput, admin: Admin, db: Db) -> MemberOutput:
    member = Member(**payload.model_dump())
    db.add(member)
    return save(db, member)


@router.put("/{member_id}")
def update_member(member_id: int, payload: MemberInput, admin: Admin, db: Db) -> MemberOutput:
    member = get_member(db, member_id)
    for field, value in payload.model_dump().items():
        setattr(member, field, value)
    return save(db, member)


@router.delete("/{member_id}", status_code=204)
def delete_member(member_id: int, admin: Admin, db: Db) -> Response:
    db.delete(get_member(db, member_id))
    db.commit()
    return Response(status_code=204)
