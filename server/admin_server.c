#include "employee_server.c"

int get_admin(int user_id, Admin *adm)
{
    int fd = open(ADMIN_FILE, O_RDONLY);
    Bool found = False;

    if (fd == -1)
        return -1;

    lseek(fd, (user_id - 1) * sizeof(Admin), SEEK_SET);

    struct flock lock = set_lock(fd, SEEK_CUR, 0, sizeof(Admin), 1);

    if (read(fd, adm, sizeof(Admin)))
        found = True;

    unlock(fd, &lock);
    close(fd);
    if (found)
        return 0;

    return -1;
}

int find_admin(char *username, Admin *adm)
{
    int fd = open(ADMIN_FILE, O_RDONLY);
    ssize_t bytes_read;
    struct flock lock;

    if (fd == -1)
        return -1;

    set_lock(fd, SEEK_SET, 0, sizeof(Admin), 1);

    while ((bytes_read = read(fd, adm, sizeof(Admin))) > 0)
    {
        unlock(fd, &lock);
        if (are_equal(username, adm->username))
            break;
        lock = set_lock(fd, SEEK_CUR, 0, sizeof(Admin), 1);
    }

    close(fd);

    if (bytes_read == 0)
        return -1;

    return 0;
}

int update_admin(Admin *adm)
{
    int fd = open(ADMIN_FILE, O_WRONLY);

    if (fd == -1)
        return -1;

    lseek(fd, (adm->admin_id - 1) * sizeof(Admin), SEEK_SET);

    struct flock lock = set_lock(fd, SEEK_CUR, 0, sizeof(Admin), 0);

    if (write(fd, adm, sizeof(Admin)) == 0)
    {
        unlock(fd, &lock);
        close(fd);
        return -1;
    }

    unlock(fd, &lock);

    close(fd);

    return 0;
}

void login_admin(char *username, char *password, Response *res)
{
    Admin adm;

    if (find_admin(username, &adm) == -1)
    {
        strcpy(res->body, "Invalid Username / Password");
        return;
    }

    if (!are_equal(adm.password, password))
    {
        strcpy(res->body, "Invalid Username / Password");
        return;
    }

    if (adm.in_session)
    {
        strcpy(res->body, "One session is already active");
        return;
    }
    // Bool logged_in = False;
    // int fd = open(ADMIN_FILE, O_RDONLY);

    // if (fd == -1)
    // {
    //     printf("Admin file could not be opened\n");
    //     return;
    // }

    // while (read(fd, &adm, sizeof(Admin)) > 0)
    // {
    //     if (strcmp(username, adm.username))
    //         continue;
    //     if (!strcmp(password, adm.password))
    //         logged_in = True;
    //     break;
    // }

    // if (!logged_in)
    // {
    //     strcpy(res->body, "Invalid username / password\n");
    //     close(fd);
    //     return;
    // }

    adm.in_session = True;

    if (update_admin(&adm) == -1)
    {
        strcpy(res->body, "Could not login. Please try again");
        return;
    }

    res->user.user_id = adm.admin_id;
    res->user.user_type = ADMIN;
    strcpy(res->user.username, username);
    snprintf(res->body, RES_BODY_SIZE - 1, "\nLogin Successful\n");

    // close(fd);
}

void logout_admin(Response *res)
{
    Admin adm;

    if (get_admin(res->user.user_id, &adm) == -1)
    {
        strcpy(res->body, "Could not logout. Try again later.");
        return;
    }

    adm.in_session = False;

    if (update_admin(&adm) == -1)
    {
        strcpy(res->body, "Could not logout. Try again later.");
        return;
    }

    logout(res);
}

void add_new_employee(Response *res, char *username, char *password)
{
    Record *record = get_record();
    Employee emp;

    if (record == NULL)
    {
        strcpy(res->body, "Could not get access records file\n");
        return;
    }

    if (find_employee(username, &emp) != -1)
    {
        strcpy(res->body, "Username already exists. Please try again.");
        return;
    }

    int fd = open(EMPLOYEES_FILE, O_WRONLY);

    struct flock lock = set_lock(fd, SEEK_SET, 0, 0, 0);

    record->employees_count++;

    if (!update_record(record))
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
    new_emp.employee_id = record->employees_count;
    new_emp.role = REGULAR;
    new_emp.in_session = False;
    strcpy(new_emp.username, username);
    strcpy(new_emp.password, password);

    if (write(fd, &new_emp, sizeof(Employee)) < 0)
    {
        record->employees_count--;
        update_record(record);
        strcpy(res->body, "Could not create the employee\n");
        return;
    }

    strcpy(res->body, "Bank Employee Created Successfully\n");

    unlock(fd, &lock);

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

void change_admin_password(Response *res, char *new_password)
{
    Admin adm;

    if (get_admin(res->user.user_id, &adm) == -1)
    {
        strcpy(res->body, "Could not change the password");
        return;
    }

    strcpy(adm.password, new_password);

    if (update_admin(&adm) == -1)
    {
        strcpy(res->body, "Could not change the password");
        return;
    }

    strcpy(res->body, "Password Changed Successfully");
}

void manage_user_roles(Response *res, int emp_id, int role)
{
    Employee emp;

    if (get_employee(emp_id, &emp) == -1)
    {
        strcpy(res->body, "Could not find the employee with the given ID");
        return;
    }

    if (role == 1)
        emp.role = REGULAR;
    else if (role == 2)
        emp.role = MANAGER;

    if (update_employee(&emp) == -1)
    {
        strcpy(res->body, "Could not change the role for the given user");
        return;
    }

    strcpy(res->body, "Role changed successfully");
}

void handle_admin_requests(char **argv, Response *res, int argc)
{
    if (strcmp(argv[0], "LOGOUT") == 0)
    {
        logout_admin(res);
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
    else if (are_equal(argv[0], "PASSWORD_CHANGE"))
    {
        change_admin_password(res, argv[1]);
    }
    else if (are_equal(argv[0], "CHANGE_USER_ROLE"))
    {
        manage_user_roles(res, atoi(argv[1]), atoi(argv[2]));
    }
}
