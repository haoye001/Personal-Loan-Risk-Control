CREATE TABLE IF NOT EXISTS repayment_plan (
    id INT NOT NULL AUTO_INCREMENT,
    application_id INT NOT NULL,
    installment_no INT NOT NULL,
    principal_amount DECIMAL(12, 2) NOT NULL,
    interest_amount DECIMAL(12, 2) NOT NULL DEFAULT 0.00,
    due_amount DECIMAL(12, 2) NOT NULL,
    paid_amount DECIMAL(12, 2) NOT NULL DEFAULT 0.00,
    remaining_amount DECIMAL(12, 2) NOT NULL,
    due_date DATE NOT NULL,
    status VARCHAR(16) NOT NULL DEFAULT 'pending',
    PRIMARY KEY (id),
    UNIQUE KEY uq_repayment_application_installment
        (application_id, installment_no),
    KEY idx_repayment_application (application_id)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS repayment_payment (
    id BIGINT NOT NULL AUTO_INCREMENT,
    repayment_plan_id INT NOT NULL,
    amount DECIMAL(12, 2) NOT NULL,
    paid_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id),
    KEY idx_repayment_payment_plan (repayment_plan_id)
) ENGINE=InnoDB;
