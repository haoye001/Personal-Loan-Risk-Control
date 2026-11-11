from collections.abc import Iterator

from sqlalchemy import create_engine, event
from sqlalchemy.orm import Session

from app.core import DATABASE_URL

engine = create_engine(DATABASE_URL, pool_pre_ping=True, hide_parameters=True)

if engine.dialect.name == "sqlite":
    @event.listens_for(engine, "connect")
    def enable_foreign_keys(connection, _):
        connection.execute("PRAGMA foreign_keys = ON")


def get_db() -> Iterator[Session]:
    with Session(engine) as session:
        yield session
