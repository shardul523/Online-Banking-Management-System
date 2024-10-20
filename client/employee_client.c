#include "common_client.c"

void display_employee_menu()
{
    printf("1. Add New Customer\n");
    printf("2. Modify Customer Details\n");
    printf("3. Approve/Reject Loans\n");
    printf("4. View Assigned Loan Applications\n");
    printf("5. View Customer Transactions\n");
    printf("6. Change Password\n");
    printf("7. Logout\n");
}

void display_manager_menu()
{
    printf("1. Activate/Deactivate Customer Accounts\n");
    printf("2. View Loan Applications\n");
    printf("3. Assign Loan Application Processes to Employees\n");
    printf("4. Review Customer Feedback\n");
    printf("5. Change Password\n");
    printf("6. Logout\n");
}

void change_employee_password(int sock_fd, Token *user)
{
    Request req;
    Response res;
    char new_pass[PASSWORD_SIZE];

    req.user = *user;
    req.argc = 2;

    printf("Enter the new password: ");
    scanf("%s", new_pass);

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "PASSWORD_CHANGE %s", new_pass);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0) return;
    if (read(sock_fd, &res, sizeof(Response)) < 0) return;

    printf("\n%s\n", res.body);
}

// REGULAR EMPLOYEE FUNCTIONS

void add_new_customer(int sock_fd, Token *user)
{
    Request req;
    Response res;

    char username[USERNAME_SIZE], password[PASSWORD_SIZE];
    double init_balance;

    printf("\nEnter the username of the new customer: ");
    scanf("%s", username);

    printf("\nEnter the password of the new customer: ");
    scanf("%s", password);

    printf("\nEnter the initial balance of the customer: ");
    scanf("%lf", &init_balance);

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "ADD_CUSTOMER %s %s %lf", username, password, init_balance);
    req.argc = 4;
    req.user = *user;

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
    {
        printf("Could not send the request to the server\n");
        return;
    }

    if (read(sock_fd, &res, sizeof(Response)) < 0)
    {
        printf("Could not recieve the response from the server\n");
        return;
    }

    printf("%s\n", res.body);
}

void modify_customer_details(int sock_fd, Token *user)
{
    Request req;
    Response res;
    int customer_id, choice;
    char *value;

    printf("\nCUSTOMER ID: ");
    scanf("%d", &customer_id);

    printf("\nWhich detail would you like to modify?\n");
    printf("1. Username\n");
    printf("2. Password\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        value = (char *)malloc(sizeof(char) * USERNAME_SIZE);
        printf("Enter the new username: ");
        scanf("%s", value);
        snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "MODIFY %d USERNAME %s", customer_id, value);
        break;

    case 2:
        value = (char *)malloc(sizeof(char) * PASSWORD_SIZE);
        printf("Enter the new password: ");
        scanf("%s", value);
        snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "MODIFY %d PASSWORD %s", customer_id, value);
        break;

    default:
        return;
    }

    req.user = *user;
    req.argc = 4;

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
        return;
    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return;

    printf("\n%s\n", res.body);
}

void handle_loans(int sock_fd, Token* user)
{
    Request req;
    Response res;
    int loan_id;
    int choice;

    printf("Enter the Loan ID of the loan: ");
    scanf("%d", &loan_id);

    printf("\nWhat action would you like to take ?\n");
    printf("\n1. APPROVE");
    printf("\n2. REJECT");
    printf("\nEnter your choice: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > 2) {
        printf("Invalid choice\n");
        return;
    }

    req.argc = 3;
    req.user = *user;

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "LOAN_ACTION %d %d", loan_id, choice);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
        return;
    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return;

    printf("\n%s\n", res.body);    
}

void view_assigned_loan_applications(int sock_fd, Token *user)
{
    Request req;
    Response res;

    req.argc = 1;
    req.user = *user;

    strcpy(req.arguments, "VIEW_ASSIGNED_LOAN_APPLICATIONS");

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
        return;
    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return;

    printf("\n%s\n", res.body);      
}
// MANAGER FUNCTIONS

void activate_customer(int sock_fd, Token *user)
{
    int customer_id;
    Request req;
    Response res;

    printf("\nEnter the ID of the customer to be activated: ");
    scanf("%d", &customer_id);

    req.user = *user;
    req.argc = 2;
    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "ACTIVATE_CUSTOMER %d", customer_id);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
        return;
    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return;

    printf("\n%s\n", res.body);
}

void deactivate_customer(int sock_fd, Token *user)
{
    int customer_id;
    Request req;
    Response res;

    printf("\nEnter the ID of the customer to be deactivated: ");
    scanf("%d", &customer_id);

    req.user = *user;
    req.argc = 2;
    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "DEACTIVATE_CUSTOMER %d", customer_id);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
        return;
    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return;

    printf("\n%s\n", res.body);
}

void view_loan_applications(int sock_fd, Token *user)
{
    Request req;
    Response res;

    req.argc = 1;
    req.user = *user;
    strcpy(req.arguments, "VIEW_LOAN_APPLICATIONS");

    if (send(sock_fd, &req, sizeof(Request), 0) <= 0)
    {
        printf("Request failed\n");
        return;
    }
    if (read(sock_fd, &res, sizeof(Response)) <= 0)
    {
        printf("Request failed\n");
        return;
    }

    printf("\n%s\n", res.body);
}

void assign_loan_application(int sock_fd, Token *user)
{
    Request req;
    Response res;
    int loan_id;
    char employee_name[USERNAME_SIZE];

    printf("Enter the ID of the loan to be assigned: ");
    scanf("%d", &loan_id);

    printf("Enter the username of the employee to assign the loan: ");
    scanf("%s", employee_name);

    req.argc = 3;
    req.user = *user;

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "ASSIGN_LOAN %d %s", loan_id, employee_name);

    if (send(sock_fd, &req, sizeof(Request), 0) <= 0)
    {
        printf("Request failed\n");
        return;
    }
    if (read(sock_fd, &res, sizeof(Response)) <= 0)
    {
        printf("Request failed\n");
        return;
    }

    printf("\n%s\n", res.body);
}

void review_customer_feedback(int sock_fd, Token *user)
{
    Request req;
    Response res;

    req.argc = 1;
    req.user = *user;
    strcpy(req.arguments, "REVIEW_CUSTOMER_FEEDBACK");

    if (send(sock_fd, &req, sizeof(Request), 0) <= 0)
    {
        printf("Request failed\n");
        return;
    }
    if (read(sock_fd, &res, sizeof(Response)) <= 0)
    {
        printf("Request failed\n");
        return;
    }

    printf("\n%s\n", res.body);
}

void regular_employee_handler(int sock_fd, Token *user)
{
    while (1)
    {
        int choice;

        printf("Hello, %s\n", user->username);
        display_employee_menu();

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            add_new_customer(sock_fd, user);
            break;

        case 2:
            modify_customer_details(sock_fd, user);
            break;

        case 3:
            handle_loans(sock_fd, user);
            break;

        case 4:
            view_assigned_loan_applications(sock_fd, user);
            break;

        case 6:
            change_employee_password(sock_fd, user);
            break;
        case 7:
            logout(sock_fd, user);
            return;

        default:
            return;
        }
    }
}

void manager_employee_handler(int sock_fd, Token *user)
{
    while (1)
    {
        int choice;

        printf("Hello, %s\n", user->username);
        display_manager_menu();

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            int c;
            printf("\n1. Activate Customer");
            printf("\n2. Deactivate Customer\n");
            printf("\nEnter your choice: ");
            scanf("%d", &c);

            if (c == 1)
                activate_customer(sock_fd, user);
            else if (c == 2)
                deactivate_customer(sock_fd, user);
            else
                printf("INVALID CHOICE\n");
            break;

        case 2:
            view_loan_applications(sock_fd, user);
            break;

        case 3:
            assign_loan_application(sock_fd, user);
            break;

        case 4:
            review_customer_feedback(sock_fd, user);
            break;

        case 5:
            change_employee_password(sock_fd, user);
            break;

        case 6:
            logout(sock_fd, user);
            return;

        default:
            return;
        }
    }
}

void employee_handler(int sock_fd, Token *user)
{
    if (user->role == REGULAR)
        regular_employee_handler(sock_fd, user);
    else if (user->role == MANAGER)
        manager_employee_handler(sock_fd, user);
}
