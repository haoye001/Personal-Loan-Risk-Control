import argparse
from getpass import getpass

from sqlalchemy.orm import Session

from app.core import hash_password
from app.db import engine
from app.models import LoanProduct, User
from app.repositories import audit, find_user
from app.schemas.auth import RegisterRequest


def main() -> None:
    parser = argparse.ArgumentParser()
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("seed-products")
    admin = commands.add_parser("create-admin")
    admin.add_argument("username")
    admin.add_argument("phone")
    args = parser.parse_args()

    with Session(engine) as db:
        if args.command == "seed-products":
            if db.get(LoanProduct, 1) is None:
                db.add(LoanProduct(id=1, name="信用贷", max_amount=50000, term=12))
        else:
            password = getpass("Admin password: ")
            try:
                payload = RegisterRequest(username=args.username, phone=args.phone, password=password)
            except ValueError:
                parser.error("Invalid username, phone or password")
            if find_user(db, payload.username) is not None:
                parser.error("Username already exists")
            user = User(username=payload.username.lower(), phone=payload.phone,
                        password_hash=hash_password(payload.password), role="admin")
            db.add(user)
            db.flush()
            audit(db, user.id, "create_admin")
        db.commit()


if __name__ == "__main__":
    main()
