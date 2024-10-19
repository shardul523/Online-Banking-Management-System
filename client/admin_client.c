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

        case 5:
            logout(sock_fd, user);
            return;

        default:
            return;
        }
    }
}