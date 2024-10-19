#include "common_server.c"
#include "../globals.h"

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

    if (!update_record(new_uid, record->employees_count, record->admins_count))
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
        update_record(record->customers_count, record->employees_count, record->admins_count);
        strcpy(res->body, "Could not create the user\n");
        return;
    }

    strcpy(res->body, "Customer Account Created Successfully\n");

    close(customer_fd);
}

void handle_employee_requests(char **argv, Response *res)
{
    if (strcmp(argv[0], "LOGOUT") == 0)
    {
        logout(res);
    }
    else if (strcmp(argv[0], "ADD_CUSTOMER") == 0)
    {
        add_new_customer(res, argv[1], argv[2], atof(argv[3]));
    }
}