"""Add an independent member directory."""

import sqlalchemy as sa
from alembic import op

revision = "2c712bd3f652"
down_revision = "cd0a1733b397"
branch_labels = None
depends_on = None


def upgrade() -> None:
    op.create_table(
        "members",
        sa.Column("id", sa.Integer(), primary_key=True),
        sa.Column("name", sa.String(50), nullable=False),
        sa.Column("account", sa.String(50), nullable=False, unique=True),
        sa.Column("phone", sa.String(11), nullable=False),
        sa.Column("role", sa.String(30), nullable=False),
    )


def downgrade() -> None:
    op.drop_table("members")
