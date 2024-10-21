#include "customer_server.c"

extern char *loan_type_names[];
extern char *loan_status_names[];

int get_employee(int user_id, Employee *emp)
{
    int fd = open(EMPLOYEES_FILE, O_RDONLY);
    Bool found = False;

    if (fd == -1)
        return -1;

    lseek(fd, (user_id - 1) * sizeof(Employee), SEEK_SET);

    struct flock lock = set_lock(fd, SEEK_CUR, 0, sizeof(Employee), 1);

    if (read(fd, emp, sizeof(Employee)))
        found = True;

    unlock(fd, &lock);

    close(fd);
    if (found)
        return 0;

    return -1;
}

int update_employee(Employee *emp)
{
    int fd = open(EMPLOYEES_FILE, O_WRONLY);

    if (fd == -1)
        return -1;

    lseek(fd, (emp->employee_id - 1) * sizeof(Employee), SEEK_SET);

    struct flock lock = set_lock(fd, SEEK_CUR, 0, sizeof(Employee), 1);

    if (write(fd, emp, sizeof(Employee)) == 0)
    {
        unlock(fd, &lock);
        close(fd);
        return -1;
    }

    unlock(fd, &lock);

    close(fd);

    return 0;
}

int find_employee(char *username, Employee *emp)
{
    int fd = open(EMPLOYEES_FILE, O_RDONLY);
    ssize_t bytes_read;

    if (fd == -1)
        return -1;

    struct flock lock = set_lock(fd, SEEK_SET, 0, sizeof(Employee), 1);

    while ((bytes_read = read(fd, emp, sizeof(Employee))) > 0)
    {
        unlock(fd, &lock);
        if (are_equal(emp->username, username))
            break;
        lock = set_lock(fd, SEEK_CUR, 0, sizeof(Employee), 0);
    }

    close(fd);

    if (bytes_read == 0)
        return -1;

    return 0;
}

int approve_loan(Loan *loan)
{
    Customer cust;

    if (get_customer(loan->customer_id, &cust) == -1)
        return -1;

    cust.balance += loan->amount;

    if (update_customer(&cust) == -1)
        return -1;

    return 0;
}

void login_employee(char *username, char *password, Response *res)
{
    Employee e;

    if (find_employee(username, &e) == -1)
    {
        strcpy(res->body, "Invalid Username / Password");
        return;
    }

    if (!are_equal(e.password, password))
    {
        strcpy(res->body, "Invalid Username / Password");
        return;
    }

    if (e.in_session)
    {
        strcpy(res->body, "Another session already active");
        return;
    }

    e.in_session = True;

    if (update_employee(&e) == -1)
    {
        strcpy(res->body, "Could not login");
        return;
    }

    res->user.user_id = e.employee_id;
    res->user.user_type = EMPLOYEE;
    res->user.role = e.role;
    strcpy(res->user.username, username);
    snprintf(res->body, RES_BODY_SIZE - 1, "\nLogin Successful\n");
}

void logout_employee(Response *res)
{
    Employee e;

    if (get_employee(res->user.user_id, &e) == -1)
    {
        strcpy(res->body, "There was an error in logging out");
        return;
    }

    e.in_session = False;

    if (update_employee(&e) == -1)
    {
        strcpy(res->body, "There was an error in logging out");
        return;
    }

    logout(res);
}

void change_employee_password(Response *res, char *new_password)
{
    Employee emp;

    if (get_employee(res->user.user_id, &emp) == -1)
    {
        strcpy(res->body, "Could not change the password");
        return;
    }

    strcpy(emp.password, new_password);

    if (update_employee(&emp) == -1)
    {
        strcpy(res->body, "Could not change the password");
        return;
    }

    strcpy(res->body, "Password Changed Successfully");
}

// REGULAR EMPLOYEE FUNCTIONS

void add_new_customer(Response *res, char *username, char *password, double balance)
{
    Record *record = get_record();

    if (record == NULL)
    {
        strcpy(res->body, "Could not get access records file\n");
        return;
    }

    int customer_fd = open(CUSTOMERS_FILE, O_WRONLY);

    Customer cust;

    if (find_customer(username, &cust) != -1)
    {
        strcpy(res->body, "Username already taken. Please try again.");
        return;
    }

    struct flock lock = set_lock(customer_fd, SEEK_SET, 0, 0, 0);

    record->customers_count++;

    if (!update_record(record))
    {
        strcpy(res->body, "Could not write to the records file\n");
        return;
    }

    if (lseek(customer_fd, 0, SEEK_END) < 0)
    {
        strcpy(res->body, "Could not open the customers file\n");
        return;
    }

    Customer new_cust;
    new_cust.customer_id = record->customers_count;
    new_cust.balance = balance;
    new_cust.in_session = False;
    new_cust.is_active = True;
    strcpy(new_cust.username, username);
    strcpy(new_cust.password, password);

    if (write(customer_fd, &new_cust, sizeof(Customer)) < 0)
    {
        record->customers_count--;
        update_record(record);
        strcpy(res->body, "Could not create the user\n");
        return;
    }

    unlock(customer_fd, &lock);

    strcpy(res->body, "Customer Account Created Successfully\n");

    close(customer_fd);
}

void modify_customer_details(Response *res, int customer_id, char *field, char *value)
{
    Customer cust;

    if (get_customer(customer_id, &cust) == -1)
    {
        strcpy(res->body, "Could not get customer details");
        return;
    }

    if (strcmp(field, "USERNAME") == 0)
        strcpy(cust.username, value);
    else if (strcmp(field, "PASSWORD") == 0)
        strcpy(cust.password, value);

    if (update_customer(&cust) == -1)
    {
        strcpy(res->body, "Could not modify customer details");
        return;
    }

    snprintf(res->body, MAX_ARGUMENT_SIZE - 1, "\nCUSTOMER DETAILS MODIFIED\nNEW MODIFIED %s: %s\n", field, value);
}

void handle_loan(Response *res, int loan_id, int action)
{
    Loan loan;

    if (get_loan(loan_id, &loan) == -1)
    {
        strcpy(res->body, "Loan action could not be completed. Please try again.");
        return;
    }

    if (action == 1)
    {
        loan.status = GRANTED;
        if (approve_loan(&loan) == -1)
        {
            strcpy(res->body, "Loan could not be granted successfully");
            return;
        }
    }
    else if (action == 2)
        loan.status = REJECTED;

    if (update_loan(&loan) == -1)
    {
        strcpy(res->body, "Loan action could not be completed. Please try again.");
        return;
    }

    snprintf(res->body, RES_BODY_SIZE - 1, "Loan %s Successfully", loan_status_names[loan.status]);
}

void view_assigned_loan_applications(Response *res)
{
    int fd = open(LOANS_FILE, O_RDONLY);
    Loan curr_loan;
    char curr_line[100];
    strcpy(res->body, "There was an error while fetching loan applications");

    if (fd == -1)
        return;

    snprintf(res->body, RES_BODY_SIZE - 1, "LOAN ID\t LOAN TYPE\t CUSTOMER ID\t LOAN STATUS\n");

    struct flock lock = set_lock(fd, SEEK_SET, 0, sizeof(Loan), 1);

    while (read(fd, &curr_loan, sizeof(Loan)) > 0)
    {
        unlock(fd, &lock);
        if (curr_loan.employee_id == res->user.user_id)
        {
            snprintf(curr_line, 99, "%-7d\t %-9s\t %-11d\t %-10s\n", curr_loan.loan_id, loan_type_names[curr_loan.type],
                     curr_loan.customer_id, loan_status_names[curr_loan.status]);
            strcat(res->body, curr_line);
        }
    }
}

// MANAGER EMPLOYEE FUNCTIONS

void activate_customers(Response *res, int customer_id)
{
    Customer cust;

    if (get_customer(customer_id, &cust) == -1)
    {
        strcpy(res->body, "Could not get customer details");
        return;
    }

    if (cust.is_active)
    {
        strcpy(res->body, "Customer account is already active");
        return;
    }

    cust.is_active = True;

    update_customer(&cust);

    strcpy(res->body, "Customer account activated");
}

void deactivate_customers(Response *res, int customer_id)
{
    Customer cust;

    if (get_customer(customer_id, &cust) == -1)
    {
        strcpy(res->body, "Could not get customer details");
        return;
    }

    if (!cust.is_active)
    {
        strcpy(res->body, "Customer account is already deactive");
        return;
    }

    cust.is_active = False;

    update_customer(&cust);

    strcpy(res->body, "Customer account deactivated");
}

void view_loan_applications(Response *res)
{
    int fd = open(LOANS_FILE, O_RDONLY);

    Loan curr_loan;
    char curr_line[100];

    snprintf(res->body, MAX_ARGUMENT_SIZE - 1, "LOAN ID\t CUSTOMER ID\t EMPLOYEE_ID\t LOAN TYPE\t LOAN AMOUNT\t LOAN STATUS\n");

    while (read(fd, &curr_loan, sizeof(Loan)) > 0)
    {
        snprintf(curr_line, 99, "%-7d\t %-11d\t %-11d\t %-9s\t %-10lf\t %-10s\n", curr_loan.loan_id, curr_loan.customer_id, curr_loan.employee_id, loan_type_names[1], curr_loan.amount, loan_status_names[curr_loan.status]);
        strcat(res->body, curr_line);
    }
}

void assign_loan_applications(Response *res, int loan_id, char *emp_name)
{
    Employee emp;
    Loan loan;

    if (find_employee(emp_name, &emp) == -1)
    {
        strcpy(res->body, "Could not find the given bank employee");
        return;
    }

    if (emp.role == MANAGER)
    {
        strcpy(res->body, "The given employee is a Manager. Please choose someone else");
        return;
    }

    if (get_loan(loan_id, &loan) == -1)
    {
        strcpy(res->body, "Could not get loan details");
        return;
    }

    loan.employee_id = emp.employee_id;

    if (update_loan(&loan) == -1)
    {
        strcpy(res->body, "Could not assign loan to employee");
        return;
    }

    strcpy(res->body, "Loan assigned successfully");
}

void review_customer_feedback(Response *res)
{
    int fd = open(CUSTOMERS_FILE, O_RDONLY);
    Customer curr;
    char curr_line[FEEDBACK_SIZE + 100];

    if (fd == -1)
    {
        strcpy(res->body, "Could not process customer feedbacks");
        return;
    }

    strcpy(res->body, "");

    struct flock lock = set_lock(fd, SEEK_SET, 0, sizeof(Customer), 1);

    while (read(fd, &curr, sizeof(Customer)) > 0)
    {
        unlock(fd, &lock);
        if (strlen(curr.feedback) > 0)
        {
            snprintf(curr_line, FEEDBACK_SIZE + 100, "Username: %s\n\n", curr.username);
            strcat(curr_line, curr.feedback);
            strcat(res->body, curr_line);
        }

        lock = set_lock(fd, SEEK_CUR, 0, sizeof(Customer), 1);
    }
}

void handle_regular_employee_requests(char **argv, Response *res)
{
    if (strcmp(argv[0], "LOGOUT") == 0)
    {
        logout_employee(res);
    }
    else if (strcmp(argv[0], "ADD_CUSTOMER") == 0)
    {
        add_new_customer(res, argv[1], argv[2], atof(argv[3]));
    }
    else if (are_equal(argv[0], "MODIFY"))
    {
        modify_customer_details(res, atoi(argv[1]), argv[2], argv[3]);
    }
    else if (are_equal(argv[0], "LOAN_ACTION"))
    {
        handle_loan(res, atoi(argv[1]), atoi(argv[2]));
    }
    else if (are_equal(argv[0], "VIEW_ASSIGNED_LOAN_APPLICATIONS"))
    {
        view_assigned_loan_applications(res);
    }
    else if (are_equal(argv[0], "PASSWORD_CHANGE"))
    {
        change_employee_password(res, argv[1]);
    }
    else if (are_equal(argv[0], "VIEW_CUSTOMER_TRANSACTIONS"))
    {
        send_transactions_by_id(atoi(argv[1]), res);
    }
}

void handle_manager_requests(char **argv, Response *res)
{
    if (are_equal(argv[0], "LOGOUT"))
    {
        logout_employee(res);
    }
    else if (are_equal(argv[0], "ACTIVATE_CUSTOMER"))
    {
        activate_customers(res, atoi(argv[1]));
    }
    else if (are_equal(argv[0], "DEACTIVATE_CUSTOMER"))
    {
        deactivate_customers(res, atoi(argv[1]));
    }
    else if (are_equal(argv[0], "VIEW_LOAN_APPLICATIONS"))
    {
        view_loan_applications(res);
    }
    else if (are_equal(argv[0], "ASSIGN_LOAN"))
    {
        assign_loan_applications(res, atoi(argv[1]), argv[2]);
    }
    else if (are_equal(argv[0], "PASSWORD_CHANGE"))
    {
        change_employee_password(res, argv[1]);
    }
    else if (are_equal(argv[0], "REVIEW_CUSTOMER_FEEDBACK"))
    {
        review_customer_feedback(res);
    }
}

void handle_employee_requests(char **argv, Response *res, int argc)
{
    if (res->user.role == REGULAR)
        handle_regular_employee_requests(argv, res);
    else if (res->user.role == MANAGER)
        handle_manager_requests(argv, res);
    else
        strcpy(res->body, "\nThere was an error in processing the request\n");
}
