#include <stdio.h>
#include <string.h>

#include "common_client.c"

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

int view_account_balance(int sock_fd, Token *user)
{
    Request req;
    Response res;

    req.user = *user;
    req.argc = 1;
    strcpy(req.arguments, "GET_BALANCE");

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
        return -1;

    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return -1;

    printf("%s\n", res.body);

    return 0;
}

void withdraw_money(int sock_fd, Token* user)
{
    Request req;
    Response res;
    double amount;

    printf("Enter the amount you would like to withdraw: ");
    scanf("%lf", &amount);

    req.user = *user;
    req.argc = 2;
    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "WITHDRAW %lf", amount);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0) return;

    if (read(sock_fd, &res, sizeof(Response)) < 0) return;

    printf("\n%s\n", res.body);
}

void customer_handler(int sock_fd, Token *user)
{
    int choice;

    while (1)
    {
        display_customer_menu();
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            // printf("Get customer balance\n");
            if (view_account_balance(sock_fd, user) == -1)
                printf("Please try again later!\n");
            break;

        case 2:
            withdraw_money(sock_fd, user);
            break;

        case 3:
            printf("Deposit money in customer's account\n");
            break;

        case 4:
            printf("Transfer funds to another account\n");
            break;

        case 9:
            logout(sock_fd, user);
            return;
        default:
            printf("Operation Not Found\n");
            break;
        }
    }
}
