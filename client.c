#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "globals.h"

int login(int socket_fd, Token *user, char *username, char *password, UserType type);
void display_customer_menu();

void display_employee_menu();
void display_manager_menu();

void display_admin_menu();

int connect_server()
{
    struct sockaddr_in server_address;
    int socket_fd;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0)
        return -1;

    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        return -1;

    return socket_fd;
}

int main()
{
    int socket_fd = connect_server();

    if (socket_fd == -1)
    {
        perror("Could not connect to the server");
        exit(EXIT_FAILURE);
    }

    char username[USERNAME_SIZE], password[PASSWORD_SIZE];
    Token *user = (Token *)malloc(sizeof(Token));
    int user_type;

    user->user_id = -1;

    printf("LOGIN\n");
    printf("1. CUSTOMER\n");
    printf("2. EMPLOYEE\n");
    printf("3. ADMIN\n");

    printf("Enter user type: ");
    scanf("%d", &user_type);

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    switch (user_type)
    {
    case CUSTOMER:
        if (login(socket_fd, user, username, password, CUSTOMER) < 0)
            break;
        display_customer_menu();
        break;
    case EMPLOYEE:
        login(socket_fd, user, username, password, EMPLOYEE);
        display_employee_menu();
        break;
    case ADMIN:
        login(socket_fd, user, username, password, ADMIN);
        display_admin_menu();
        break;
    default:
        break;
    }

    close(socket_fd);
}

// CUSTOMER FUNCTIONS

int login(int socket_fd, Token *user, char *username, char *password, UserType type)
{
    Request login_req;
    Response login_res;

    login_req.argc = 4;
    login_req.user = *user;

    if (type == CUSTOMER)
        snprintf(login_req.arguments, MAX_ARGUMENT_SIZE - 1, "LOGIN CUSTOMER %s %s", username, password);
    else if (type == EMPLOYEE)
        snprintf(login_req.arguments, MAX_ARGUMENT_SIZE - 1, "LOGIN EMPLOYEE %s %s", username, password);
    else if (type == ADMIN)
        snprintf(login_req.arguments, MAX_ARGUMENT_SIZE - 1, "LOGIN ADMIN %s %s", username, password);

    if (send(socket_fd, &login_req, sizeof(Request), 0) < 0)
        return -1;

    if (read(socket_fd, &login_res, sizeof(Response)) < 0)
        return -1;

    *user = login_res.user;

    if (user->user_type != NONE)
        printf("%s\n", login_res.body);

    return 0;
}

void display_customer_menu()
{
    printf("1. View Account Balance\n");
    printf("2. Withdraw Money\n");
    printf("3. Deposit Money\n");
    printf("4. Transfer Funds\n");
    printf("5. View Transaction History\n");
    printf("6. Apply for a Loan\n");
    printf("7. Adding Feedback\n");
    printf("8. Change Password\n");
    printf("9. Logout\n");
}

int employee_login(char *username, char *password)
{
    return 0;
}

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

Admin *admin_login(char *username, char *password)
{
    return 0;
}

void display_admin_menu()
{
    printf("1. Add New Bank Employee\n");
    printf("2. Modify Customer/Employee Details\n");
    printf("3. Manage User Roles\n");
    printf("4. Change Password\n");
    printf("5. Logout\n");
}