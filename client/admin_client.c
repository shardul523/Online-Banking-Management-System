#include "common_client.c"

void display_admin_menu()
{
    printf("1. Add New Bank Employee\n");
    printf("2. Modify Customer/Employee Details\n");
    printf("3. Manage User Roles\n");
    printf("4. Change Password\n");
    printf("5. Logout\n");
}

void add_new_employee(int sock_fd, Token *user)
{
    Request req;
    Response res;

    char username[USERNAME_SIZE], password[PASSWORD_SIZE];

    printf("\nEnter the username of the new employee: ");
    scanf("%s", username);

    printf("\nEnter the password of the new employee: ");
    scanf("%s", password);

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "ADD_EMPLOYEE %s %s", username, password);
    req.argc = 3;
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

void modify_detalis(int sock_fd, Token *user, UserType user_type)
{
    Request req;
    Response res;
    int choice, user_id;
    char field[10], *value;

    req.user = *user;
    
    printf("\nEnter the ID of the user: ");
    scanf("%d", &user_id);
    printf("\n1. USERNAME\n");
    printf("2. PASSWORD\n");
    printf("\nEnter which detail would you like to modify: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        strcpy(field, "USERNAME");
        value = (char*) malloc(USERNAME_SIZE * sizeof(char));
    }
    else if (choice == 2)
    {
        strcpy(field, "PASSWORD");
        value = (char*) malloc(PASSWORD_SIZE * sizeof(char));
    }
    else
    {
        printf("\nInvalid choice. Please try again.\n");
        return;
    }

    printf("\nEnter the new updated value: ");
    scanf("%s", value);

    req.argc = 4;

    if (user_type == CUSTOMER)
        snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "MODIFY_CUSTOMER %d %s %s", user_id, field, value);
    else if (user_type == EMPLOYEE)
        snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "MODIFY_EMPLOYEE %d %s %s", user_id, field, value);
    else
        return;

    free(value);
    value = NULL;

    if (send(sock_fd, &req, sizeof(Request), 0) < 0) return;
    if (read(sock_fd, &res, sizeof(Response)) < 0) return;

    printf("\n%s\n", res.body);
}

void change_admin_password(int sock_fd, Token* user)
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

void admin_handler(int sock_fd, Token *user)
{
    while (1)
    {
        int choice;

        printf("Hello, %s\n", user->username);
        display_admin_menu();

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            add_new_employee(sock_fd, user);
            break;

        case 2:
            UserType type;
            printf("\n1. CUSTOMER");
            printf("\n2. Employee");
            printf("\nEnter the type of user you would like to modfiy: ");
            scanf("%d", (int*) &type);
            modify_detalis(sock_fd, user, type);
            break;

        case 5:
            logout(sock_fd, user);
            break;

        default:
            return;
        }

        if (user->user_id == -1) return;

    }
}