#include "employee_server.c"

void login_admin(char *username, char *password, Response *res)
{
    Admin adm;
    Bool logged_in = False;
    int fd = open(ADMIN_FILE, O_RDONLY);

    if (fd == -1)
    {
        printf("Admin file could not be opened\n");
        return;
    }

    while (read(fd, &adm, sizeof(Admin)) > 0)
    {
        if (strcmp(username, adm.username))
            continue;
        if (!strcmp(password, adm.password))
            logged_in = True;
        break;
    }

    if (!logged_in)
    {
        strcpy(res->body, "Invalid username / password\n");
        close(fd);
        return;
    }

    printf("Logged in successfully\n");

    res->user.user_id = adm.admin_id;
    res->user.user_type = ADMIN;
    strcpy(res->user.username, username);
    snprintf(res->body, RES_BODY_SIZE - 1, "\nLogin Successful\n");

    close(fd);
}

void add_new_employee(Response *res, char *username, char *password)
{
    Record *record = get_record();

    if (record == NULL)
    {
        strcpy(res->body, "Could not get access records file\n");
        return;
    }

    int fd = open(EMPLOYEES_FILE, O_WRONLY);

    int new_uid = record->employees_count + 1;

    if (!update_record(record->customers_count, new_uid, record->admins_count, record->loans_count))
    {
        strcpy(res->body, "Could not write to the records file\n");
        return;
    }

    if (lseek(fd, 0, SEEK_END) < 0)
    {
        strcpy(res->body, "Could not open the employees file\n");
        return;
    }

    Employee new_emp;
    new_emp.employee_id = new_uid;
    new_emp.role = REGULAR;
    strcpy(new_emp.username, username);
    strcpy(new_emp.password, password);

    if (write(fd, &new_emp, sizeof(Employee)) < 0)
    {
        update_record(record->customers_count, record->employees_count, record->admins_count, record->loans_count);
        strcpy(res->body, "Could not create the employee\n");
        return;
    }

    strcpy(res->body, "Bank Employee Created Successfully\n");

    close(fd);
}

void modify_employee_details(Response *res, int employee_id, char *field, char *value)
{
    Employee emp;

    if (get_employee(employee_id, &emp) == -1)
    {
        strcpy(res->body, "Could not get customer details");
        return;
    }

    if (strcmp(field, "USERNAME") == 0)
        strcpy(emp.username, value);
    else if (strcmp(field, "PASSWORD") == 0)
        strcpy(emp.password, value);

    if (update_employee(&emp) == -1)
    {
        strcpy(res->body, "Could not modify employee details");
        return;
    }

    snprintf(res->body, MAX_ARGUMENT_SIZE - 1, "\nEMPLOYEE DETAILS MODIFIED\nNEW MODIFIED %s: %s\n", field, value);
}

void handle_admin_requests(char **argv, Response *res)
{
    if (strcmp(argv[0], "LOGOUT") == 0)
    {
        logout(res);
    }
    else if (strcmp(argv[0], "ADD_EMPLOYEE") == 0)
    {
        add_new_employee(res, argv[1], argv[2]);
    }
    else if (are_equal(argv[0], "MODIFY_CUSTOMER"))
    {
        modify_customer_details(res, atoi(argv[1]), argv[2], argv[3]);
    }
    else if (are_equal(argv[0], "MODIFY_EMPLOYEE"))
    {
        modify_employee_details(res, atoi(argv[1]), argv[2], argv[3]);
    }
}
