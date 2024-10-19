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
    printf("2. Assign Loan Application Processes to Employees\n");
    printf("3. Review Customer Feedback\n");
    printf("4. Change Password\n");
    printf("5. Logout\n");
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

// MODIFY customer_id field value
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
            printf("Activate / Deactivate Customer Accounts\n");
            break;

        case 5:
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
