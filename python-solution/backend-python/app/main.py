from contextlib import asynccontextmanager

from fastapi import FastAPI, Request
from fastapi.exceptions import RequestValidationError
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse

from app.core import CORS_ORIGINS, validate_settings
from app.routers import auth, loans, members, repayments


@asynccontextmanager
async def lifespan(_: FastAPI):
    validate_settings()
    yield


app = FastAPI(title="个人贷款风控系统 API", version="1.0.0", lifespan=lifespan)
app.add_middleware(
    CORSMiddleware,
    allow_origins=CORS_ORIGINS,
    allow_methods=["GET", "POST", "PUT", "DELETE"],
    allow_headers=["Authorization", "Content-Type"],
)
app.include_router(auth.router, prefix="/api")
app.include_router(loans.router, prefix="/api")
app.include_router(loans.admin_router, prefix="/api")
app.include_router(repayments.router, prefix="/api")
app.include_router(members.router, prefix="/api")


@app.exception_handler(RequestValidationError)
async def validation_error(_: Request, exc: RequestValidationError) -> JSONResponse:
    return JSONResponse(
        status_code=422,
        content={"detail": [{key: error[key] for key in ("loc", "msg", "type")} for error in exc.errors()]},
    )


@app.get("/health", tags=["系统"])
def health_check() -> dict[str, str]:
    return {"status": "ok"}
