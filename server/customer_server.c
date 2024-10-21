#include "../globals.h"
#include "common_server.c"

int get_customer(int user_id, Customer *cust)
{
    int fd = open(CUSTOMERS_FILE, O_RDONLY);
    Bool found = False;

    if (fd == -1)
        return -1;

    lseek(fd, (user_id - 1) * sizeof(Customer), SEEK_SET);

    struct flock lock = set_lock(fd, SEEK_CUR, 0, sizeof(Customer), 1);

    if (read(fd, cust, sizeof(Customer)))
        found = True;

    unlock(fd, &lock);
    close(fd);
    if (found)
        return 0;

    return -1;
}

int find_customer(char *username, Customer *cust)
{
    int fd = open(CUSTOMERS_FILE, O_RDONLY);
    ssize_t bytes_read;
    struct flock lock;

    if (fd == -1)
        return -1;

    set_lock(fd, SEEK_SET, 0, sizeof(Customer), 1);

    while ((bytes_read = read(fd, cust, sizeof(Customer))) > 0)
    {
        unlock(fd, &lock);
        if (are_equal(username, cust->username))
            break;
        lock = set_lock(fd, SEEK_CUR, 0, sizeof(Customer), 1);
    }

    close(fd);

    if (bytes_read == 0)
        return -1;

    return 0;
}

int update_customer(Customer *cust)
{
    int fd = open(CUSTOMERS_FILE, O_WRONLY);

    if (fd == -1)
        return -1;

    lseek(fd, (cust->customer_id - 1) * sizeof(Customer), SEEK_SET);

    struct flock lock = set_lock(fd, SEEK_CUR, 0, sizeof(Customer), 0);

    if (write(fd, cust, sizeof(Customer)) == 0)
    {
        unlock(fd, &lock);
        close(fd);
        return -1;
    }

    unlock(fd, &lock);

    close(fd);

    return 0;
}

void logout_customer(Response *res)
{
    Customer cust;

    if (get_customer(res->user.user_id, &cust) == -1)
    {
        strcpy(res->body, "Could not logout. Please try again later.");
        return;
    }

    cust.in_session = False;

    if (update_customer(&cust) == -1)
    {
        strcpy(res->body, "Could not logout. Please try again later.");
        return;
    }

    logout(res);
}

void login_customer(char *username, char *password, Response *res)
{
    // int log_fd = open("LOG.txt", O_WRONLY);
    Customer cust;

    if (find_customer(username, &cust) == -1)
    {
        strcpy(res->body, "\nInvalid Username / Password\n");
        return;
    }

    if (!are_equal(password, cust.password))
    {
        strcpy(res->body, "\nInvalid Username / Password\n");
        return;
    }

    if (!cust.is_active)
    {
        strcpy(res->body, "\nAccount Inactive\n");
        return;
    }

    if (cust.in_session)
    {
        strcpy(res->body, "\nAnother session already active\n");
        return;
    }

    // Bool logged_in = False;
    // int fd = open(CUSTOMERS_FILE, O_RDONLY);

    // while (read(fd, &cust, sizeof(Customer)) > 0)
    // {
    //     if (strcmp(username, cust.username))
    //         continue;
    //     if (!strcmp(password, cust.password))
    //         logged_in = True;
    //     break;
    // }

    // if (!logged_in)
    // {
    //     close(fd);
    //     snprintf(res->body, RES_BODY_SIZE - 1, "Invalid Username / Password\n");
    //     return;
    // }
    cust.in_session = True;

    if (update_customer(&cust) == -1)
    {
        strcpy(res->body, "Could not login");
        return;
    }

    res->user.user_type = CUSTOMER;
    res->user.user_id = cust.customer_id;
    strcpy(res->user.username, username);
    snprintf(res->body, RES_BODY_SIZE - 1, "\nLogin Successful\n");

    // close(fd);
}

int add_loan(Loan *loan)
{
    int fd = open(LOANS_FILE, O_WRONLY | O_APPEND | O_CREAT, 0666);
    Record *record = get_record();

    if (fd == -1)
        return -1;

    struct flock lock = set_lock(fd, SEEK_SET, 0, 0, 1);
    record->loans_count++;

    if (!update_record(record))
    {
        close(fd);
        return -1;
    }

    loan->loan_id = record->loans_count;

    if (write(fd, loan, sizeof(Loan)) <= 0)
    {
        unlock(fd, &lock);
        close(fd);
        return -1;
    }

    unlock(fd, &lock);

    return 0;
}

void view_customer_balance(Response *res)
{
    Customer cust;

    if (get_customer(res->user.user_id, &cust) < 0)
    {
        snprintf(res->body, RES_BODY_SIZE - 1, "Could not get customer details\n");
        return;
    }

    snprintf(res->body, RES_BODY_SIZE - 1, "CUSTOMER BALANCE: %.2lf\n", cust.balance);
}

void withdraw_money(Response *res, double amount)
{
    Customer cust;

    if (get_customer(res->user.user_id, &cust) < 0)
    {
        strcpy(res->body, "Could not withdraw the money\n");
        return;
    }

    if (cust.balance < amount)
    {
        strcpy(res->body, "Insufficient Balance\n");
        return;
    }

    cust.balance -= amount;

    if (update_customer(&cust) == -1)
    {
        strcpy(res->body, "Could not withdraw the money");
        return;
    }

    add_transaction(res->user.user_id, -1, amount);

    snprintf(res->body, MAX_ARGUMENT_SIZE - 1, "Withdrawn amount %.2lf\nNew Balance: %.2lf", amount, cust.balance);
}

void deposit_money(Response *res, double amount)
{
    Customer cust;

    if (get_customer(res->user.user_id, &cust) < 0)
    {
        strcpy(res->body, "Could not deposit the money\n");
        return;
    }

    cust.balance += amount;

    if (update_customer(&cust) == -1)
    {
        strcpy(res->body, "Could not withdraw the money");
        return;
    }

    add_transaction(-1, res->user.user_id, amount);

    snprintf(res->body, MAX_ARGUMENT_SIZE - 1, "Deposited amount %.2lf\nNew Balance: %.2lf", amount, cust.balance);
}

void transfer_money(Response *res, char *beneficiary_name, double amount)
{
    Customer transferee, beneficiary;

    if (get_customer(res->user.user_id, &transferee) == -1)
    {
        strcpy(res->body, "\nCould not access the Transferee details\n");
        return;
    }

    if (find_customer(beneficiary_name, &beneficiary) == -1)
    {
        strcpy(res->body, "\nCould not access the Beneficiary details\n");
        return;
    }

    if (transferee.balance < amount)
    {
        strcpy(res->body, "\nInsufficient Balance for transfer\n");
        return;
    }

    transferee.balance -= amount;
    beneficiary.balance += amount;

    update_customer(&transferee);
    update_customer(&beneficiary);

    add_transaction(transferee.customer_id, beneficiary.customer_id, amount);

    snprintf(res->body, MAX_ARGUMENT_SIZE - 1, "\nAmount %.2lf transferred from %s to %s\nUpdated Balance: %.2lf\n", amount, transferee.username, beneficiary.username, transferee.balance);
}

void apply_for_loan(Response *res, int loan_type, double amount)
{
    Loan loan;

    loan.amount = amount;
    loan.customer_id = res->user.user_id;
    loan.employee_id = -1;
    loan.status = PENDING;
    loan.type = loan_type;

    if (add_loan(&loan) == -1)
    {
        strcpy(res->body, "Loan Application was not submitted");
        return;
    }

    snprintf(res->body, MAX_ARGUMENT_SIZE - 1, "Loan Application was submitted successfully");
}

void give_feedback(Response *res, char** argv, int argc) 
{
    char *feedback = join_words(argv, argc - 1);
    Customer cust;

    if (get_customer(res->user.user_id, &cust) == -1) {
        strcpy(res->body, "Could not submit feedback");
        return;
    }

    strcpy(cust.feedback, feedback);

    if (update_customer(&cust) == -1) {
        strcpy(res->body, "Could not submit feedback");
        return;
    }

    strcpy(res->body, "Feedback Submitted Successfully");
}

void change_password(Response *res, char* new_password)
{
    Customer cust;

    if (get_customer(res->user.user_id, &cust) == -1) 
    {
        strcpy(res->body, "Could not change the password");
        return;
    }

    strcpy(cust.password, new_password);

    if (update_customer(&cust) == -1) 
    {
        strcpy(res->body, "Could not change the password");
        return;
    }

    strcpy(res->body, "Password Changed Successfully");
}

void handle_customer_requests(char **argv, Response *res, int argc)
{
    if (strcmp(argv[0], "GET_BALANCE") == 0)
    {
        printf("Getting Customer Balance...\n");
        view_customer_balance(res);
    }
    else if (strcmp(argv[0], "WITHDRAW") == 0)
    {
        withdraw_money(res, atof(argv[1]));
    }
    else if (strcmp(argv[0], "DEPOSIT") == 0)
    {
        deposit_money(res, atof(argv[1]));
    }
    else if (are_equal(argv[0], "TRANSFER"))
    {
        transfer_money(res, argv[1], atof(argv[2]));
    }
    else if (are_equal(argv[0], "LOAN"))
    {
        apply_for_loan(res, atoi(argv[1]), atof(argv[2]));
    }
    else if (are_equal(argv[0], "GIVE_FEEDBACK"))
    {
        give_feedback(res, argv, argc);
    }
    else if (are_equal(argv[0], "PASSWORD_CHANGE"))
    {
        change_password(res, argv[1]);
    }
    else if (are_equal(argv[0], "GET_TRANSACTIONS"))
    {
        send_transactions_by_id(res->user.user_id, res);
    }
    else if (strcmp(argv[0], "LOGOUT") == 0)
    {
        logout_customer(res);
    }
}
