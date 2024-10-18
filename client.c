#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "globals.h"

int customer_login(int socket_fd, Token* user, char *username, char *password);
void display_customer_menu();

Employee *employee_login(char *username, char *password);
void display_employee_menu();
void display_manager_menu();

Admin *admin_login(char *username, char *password);
void display_admin_menu();
/*
METHOD - GET

ACTION -
TYPE -
ARGUEMENTS -


TOKEN -
1. USER TYPE
2. USER ID
 */

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

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
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

    int user_type = 0;
    char username[USERNAME_SIZE], password[PASSWORD_SIZE];
    Token* user = (Token*) malloc(sizeof(Token));

    user->user_id = -1;
    user->user_type = NONE;

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
        if (customer_login(socket_fd, user, username, password) < 0)
            break;
        display_customer_menu();
        break;
    case EMPLOYEE:
        employee_login(username, password);
        display_employee_menu();
        break;
    case ADMIN:
        admin_login(username, password);
        display_admin_menu();
        break;
    default:
        break;
    }

    close(socket_fd);
}

// CUSTOMER FUNCTIONS

int customer_login(int socket_fd, Token *user, char *username, char *password)
{
    Request login_req;
    Response login_res;
    Token *token = (Token*) malloc(sizeof(Token));

    login_req.argc = 3;
    login_req.user = *user;

    snprintf(login_req.arguments, MAX_ARGUMENT_SIZE - 1, "LOGIN_CUSTOMER %s %s", username, password);

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

Employee *employee_login(char *username, char *password)
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