#include "RepaymentDAO.h"

#include "../utils/Logger.h"
#include <mysql.h>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

extern MYSQL* connectDatabase();

namespace
{
    std::string formatMoney(long long cents)
    {
        std::ostringstream value;
        value << (cents / 100) << '.'
              << std::setw(2) << std::setfill('0') << (cents % 100);
        return value.str();
    }

    bool queryFailed(MYSQL* conn, const std::string& operation,
                     std::string& outError)
    {
        outError = "database error: " + operation + ": " + mysql_error(conn);
        LOG_ERROR("RepaymentDAO: " + outError);
        return false;
    }

    bool readRepayments(
        MYSQL* conn,
        int applicationId,
        std::vector<Repayment>& repayments,
        std::string& outError,
        bool forUpdate = false)
    {
        std::string sql =
            "SELECT id, application_id, installment_no, principal_amount, "
            "interest_amount, due_amount, paid_amount, remaining_amount, "
            "due_date, status FROM repayment_plan WHERE application_id = "
            + std::to_string(applicationId)
            + " ORDER BY installment_no";

        if (forUpdate)
        {
            sql += " FOR UPDATE";
        }

        if (mysql_query(conn, sql.c_str()) != 0)
        {
            return queryFailed(conn, "query repayment plan", outError);
        }

        MYSQL_RES* result = mysql_store_result(conn);
        if (result == nullptr)
        {
            return queryFailed(conn, "read repayment plan result", outError);
        }

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != nullptr)
        {
            Repayment repayment{};
            repayment.id = std::stoi(row[0]);
            repayment.applicationId = std::stoi(row[1]);
            repayment.installmentNumber = std::stoi(row[2]);
            repayment.principalAmount = row[3] ? std::stod(row[3]) : 0.0;
            repayment.interestAmount = row[4] ? std::stod(row[4]) : 0.0;
            repayment.dueAmount = row[5] ? std::stod(row[5]) : 0.0;
            repayment.paidAmount = row[6] ? std::stod(row[6]) : 0.0;
            repayment.remainingAmount = row[7] ? std::stod(row[7]) : 0.0;
            repayment.dueDate = row[8] ? row[8] : "";
            repayment.status = row[9] ? row[9] : "";
            repayments.push_back(repayment);
        }

        mysql_free_result(result);
        return true;
    }

    bool beginTransaction(MYSQL* conn, std::string& outError)
    {
        if (mysql_query(conn, "START TRANSACTION") != 0)
        {
            return queryFailed(conn, "start transaction", outError);
        }
        return true;
    }

    void rollback(MYSQL* conn)
    {
        if (mysql_query(conn, "ROLLBACK") != 0)
        {
            LOG_ERROR(std::string("RepaymentDAO: rollback failed: ")
                      + mysql_error(conn));
        }
    }

    bool commit(MYSQL* conn, std::string& outError)
    {
        if (mysql_query(conn, "COMMIT") != 0)
        {
            return queryFailed(conn, "commit transaction", outError);
        }
        return true;
    }
}

bool RepaymentDAO::createPlanAndDisburse(
    int applicationId,
    std::string& outError)
{
    MYSQL* conn = connectDatabase();
    if (conn == nullptr)
    {
        outError = "database error: unable to connect to database";
        return false;
    }

    if (!beginTransaction(conn, outError))
    {
        mysql_close(conn);
        return false;
    }

    const std::string applicationSql =
        "SELECT la.amount, la.status, lp.term "
        "FROM loan_application la "
        "JOIN loan_product lp ON lp.id = la.loan_id "
        "WHERE la.id = " + std::to_string(applicationId) + " FOR UPDATE";

    if (mysql_query(conn, applicationSql.c_str()) != 0)
    {
        queryFailed(conn, "load application for disbursement", outError);
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr)
    {
        queryFailed(conn, "read application for disbursement", outError);
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == nullptr)
    {
        mysql_free_result(result);
        outError = "application not found";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    const double applicationAmount = row[0] ? std::stod(row[0]) : 0.0;
    const std::string status = row[1] ? row[1] : "";
    const int term = row[2] ? std::stoi(row[2]) : 0;
    mysql_free_result(result);

    if (status != "approved")
    {
        outError = "only approved applications can be disbursed";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    if (term <= 0 || term > 600 || !std::isfinite(applicationAmount)
        || applicationAmount <= 0.0)
    {
        outError = "invalid loan amount or term";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    const long long totalCents =
        static_cast<long long>(std::llround(applicationAmount * 100.0));
    if (totalCents < term)
    {
        outError = "loan amount is too small for the repayment term";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    std::vector<Repayment> existingRepayments;
    if (!readRepayments(conn, applicationId, existingRepayments, outError, true))
    {
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    if (!existingRepayments.empty())
    {
        outError = "repayment plan already exists";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    const long long baseInstallment = totalCents / term;
    const long long extraCents = totalCents % term;
    for (int installment = 1; installment <= term; ++installment)
    {
        const long long principalCents =
            baseInstallment + (installment <= extraCents ? 1 : 0);
        const std::string insertSql =
            "INSERT INTO repayment_plan "
            "(application_id, installment_no, principal_amount, "
            "interest_amount, due_amount, paid_amount, remaining_amount, "
            "due_date, status) VALUES ("
            + std::to_string(applicationId) + ", "
            + std::to_string(installment) + ", "
            + formatMoney(principalCents) + ", 0.00, "
            + formatMoney(principalCents)
            + ", 0.00, " + formatMoney(principalCents)
            + ", DATE_ADD(CURDATE(), INTERVAL "
            + std::to_string(installment)
            + " MONTH), 'pending')";

        if (mysql_query(conn, insertSql.c_str()) != 0)
        {
            queryFailed(conn, "insert repayment installment", outError);
            rollback(conn);
            mysql_close(conn);
            return false;
        }
    }

    const std::string updateApplicationSql =
        "UPDATE loan_application SET status = 'disbursed' "
        "WHERE id = " + std::to_string(applicationId)
        + " AND status = 'approved'";
    if (mysql_query(conn, updateApplicationSql.c_str()) != 0)
    {
        queryFailed(conn, "update application status", outError);
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    if (mysql_affected_rows(conn) != 1)
    {
        outError = "application status changed during disbursement";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    if (!commit(conn, outError))
    {
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    mysql_close(conn);
    LOG_INFO("RepaymentDAO: generated " + std::to_string(term)
             + " installments for application "
             + std::to_string(applicationId));
    return true;
}

bool RepaymentDAO::getRepaymentsByApplicationId(
    int applicationId,
    std::vector<Repayment>& repayments,
    std::string& outError)
{
    MYSQL* conn = connectDatabase();
    if (conn == nullptr)
    {
        outError = "database error: unable to connect to database";
        return false;
    }

    const bool success =
        readRepayments(conn, applicationId, repayments, outError);
    mysql_close(conn);
    return success;
}

bool RepaymentDAO::makePayment(
    int applicationId,
    long long amountCents,
    std::vector<Repayment>& repayments,
    bool& completed,
    std::string& outError)
{
    completed = false;
    MYSQL* conn = connectDatabase();
    if (conn == nullptr)
    {
        outError = "database error: unable to connect to database";
        return false;
    }

    if (!beginTransaction(conn, outError))
    {
        mysql_close(conn);
        return false;
    }

    const std::string applicationSql =
        "SELECT status FROM loan_application WHERE id = "
        + std::to_string(applicationId) + " FOR UPDATE";
    if (mysql_query(conn, applicationSql.c_str()) != 0)
    {
        queryFailed(conn, "load application for repayment", outError);
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    MYSQL_RES* applicationResult = mysql_store_result(conn);
    if (applicationResult == nullptr)
    {
        queryFailed(conn, "read application status", outError);
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    MYSQL_ROW applicationRow = mysql_fetch_row(applicationResult);
    if (applicationRow == nullptr)
    {
        mysql_free_result(applicationResult);
        outError = "application not found";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    const std::string applicationStatus =
        applicationRow[0] ? applicationRow[0] : "";
    mysql_free_result(applicationResult);
    if (applicationStatus != "disbursed" && applicationStatus != "repaying")
    {
        outError = "application is not eligible for repayment";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    if (!readRepayments(conn, applicationId, repayments, outError, true))
    {
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    if (repayments.empty())
    {
        outError = "repayment plan not found";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    long long totalRemainingCents = 0;
    for (const Repayment& repayment : repayments)
    {
        totalRemainingCents +=
            static_cast<long long>(std::llround(repayment.remainingAmount * 100.0));
    }

    if (amountCents > totalRemainingCents)
    {
        outError = "payment exceeds outstanding balance";
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    long long unappliedCents = amountCents;
    for (Repayment& repayment : repayments)
    {
        if (unappliedCents == 0)
        {
            break;
        }

        const long long installmentRemaining =
            static_cast<long long>(std::llround(repayment.remainingAmount * 100.0));
        const long long allocation =
            std::min(unappliedCents, installmentRemaining);
        if (allocation <= 0)
        {
            continue;
        }

        const std::string updateSql =
            "UPDATE repayment_plan SET status = IF(remaining_amount - "
            + formatMoney(allocation)
            + " <= 0, 'paid', 'partial'), paid_amount = paid_amount + "
            + formatMoney(allocation)
            + ", remaining_amount = remaining_amount - "
            + formatMoney(allocation)
            + " WHERE id = " + std::to_string(repayment.id);
        if (mysql_query(conn, updateSql.c_str()) != 0)
        {
            queryFailed(conn, "update repayment installment", outError);
            rollback(conn);
            mysql_close(conn);
            return false;
        }

        const std::string paymentSql =
            "INSERT INTO repayment_payment (repayment_plan_id, amount) VALUES ("
            + std::to_string(repayment.id) + ", "
            + formatMoney(allocation) + ")";
        if (mysql_query(conn, paymentSql.c_str()) != 0)
        {
            queryFailed(conn, "record repayment payment", outError);
            rollback(conn);
            mysql_close(conn);
            return false;
        }

        const long long newRemaining = installmentRemaining - allocation;
        const long long oldPaidCents =
            static_cast<long long>(std::llround(repayment.paidAmount * 100.0));
        repayment.paidAmount = (oldPaidCents + allocation) / 100.0;
        repayment.remainingAmount = newRemaining / 100.0;
        repayment.status = newRemaining == 0 ? "paid" : "partial";
        unappliedCents -= allocation;
    }

    completed = unappliedCents == 0;
    for (const Repayment& repayment : repayments)
    {
        if (std::llround(repayment.remainingAmount * 100.0) > 0)
        {
            completed = false;
            break;
        }
    }

    const std::string updateStatusSql =
        "UPDATE loan_application SET status = '"
        + std::string(completed ? "completed" : "repaying")
        + "' WHERE id = " + std::to_string(applicationId)
        + " AND status IN ('disbursed', 'repaying')";
    if (mysql_query(conn, updateStatusSql.c_str()) != 0)
    {
        queryFailed(conn, "update application repayment status", outError);
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    if (!commit(conn, outError))
    {
        rollback(conn);
        mysql_close(conn);
        return false;
    }

    mysql_close(conn);
    LOG_INFO("RepaymentDAO: payment processed for application "
             + std::to_string(applicationId)
             + ", amount = " + formatMoney(amountCents));
    return true;
}
