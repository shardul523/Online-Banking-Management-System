#include "customer_server.c"

extern char *loan_type_names[];

int get_employee(int user_id, Employee *emp)
{
    int fd = open(EMPLOYEES_FILE, O_RDONLY);
    Bool found = False;

    if (fd == -1)
        return -1;

    lseek(fd, (user_id - 1) * sizeof(Employee), SEEK_SET);

    if (read(fd, emp, sizeof(Employee)))
        found = True;

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

    if (write(fd, emp, sizeof(Employee)) == 0)
    {
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}

void login_employee(char *username, char *password, Response *res)
{
    Employee e;
    Bool logged_in = False;
    int fd = open(EMPLOYEES_FILE, O_RDONLY);

    if (fd == -1)
    {
        printf("Emp file could not be opened\n");
        return;
    }

    while (read(fd, &e, sizeof(Employee)) > 0)
    {
        if (strcmp(username, e.username))
            continue;
        if (!strcmp(password, e.password))
            logged_in = True;
        break;
    }

    if (!logged_in)
    {
        close(fd);
        strcpy(res->body, "Invalid username / password\n");
        return;
    }

    printf("Logged in successfully\n");

    res->user.user_id = e.employee_id;
    res->user.user_type = EMPLOYEE;
    res->user.role = e.role;
    strcpy(res->user.username, username);
    snprintf(res->body, RES_BODY_SIZE - 1, "\nLogin Successful\n");

    close(fd);
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

    int new_uid = record->customers_count + 1;

    if (!update_record(new_uid, record->employees_count, record->admins_count, record->loans_count))
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
    new_cust.customer_id = new_uid;
    new_cust.balance = balance;
    strcpy(new_cust.username, username);
    strcpy(new_cust.password, password);

    if (write(customer_fd, &new_cust, sizeof(Customer)) < 0)
    {
        update_record(record->customers_count, record->employees_count, record->admins_count, record->loans_count);
        strcpy(res->body, "Could not create the user\n");
        return;
    }

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

    snprintf(res->body, MAX_ARGUMENT_SIZE - 1, "LOAN ID\t CUSTOMER ID\t LOAN TYPE\t LOAN AMOUNT\n");

    while (read(fd, &curr_loan, sizeof(Loan)) > 0)
    {
        snprintf(curr_line, 99, "%-7d\t %-11d\t %-9s\t %-10lf\n", curr_loan.loan_id, curr_loan.customer_id, loan_type_names[1], curr_loan.amount);
        strcat(res->body, curr_line);
    }
}

void handle_regular_employee_requests(char **argv, Response *res)
{
    if (strcmp(argv[0], "LOGOUT") == 0)
    {
        logout(res);
    }
    else if (strcmp(argv[0], "ADD_CUSTOMER") == 0)
    {
        add_new_customer(res, argv[1], argv[2], atof(argv[3]));
    }
    else if (are_equal(argv[0], "MODIFY"))
    {
        modify_customer_details(res, atoi(argv[1]), argv[2], argv[3]);
    }
}

void handle_manager_requests(char **argv, Response *res)
{
    if (are_equal(argv[0], "LOGOUT"))
    {
        logout(res);
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
}

void handle_employee_requests(char **argv, Response *res)
{
    if (res->user.role == REGULAR)
        handle_regular_employee_requests(argv, res);
    else if (res->user.role == MANAGER)
        handle_manager_requests(argv, res);
    else
        strcpy(res->body, "\nThere was an error in processing the request\n");
}
