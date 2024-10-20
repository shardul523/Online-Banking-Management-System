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

void withdraw_money(int sock_fd, Token *user)
{
    Request req;
    Response res;
    double amount;

    printf("Enter the amount you would like to withdraw: ");
    scanf("%lf", &amount);

    req.user = *user;
    req.argc = 2;
    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "WITHDRAW %lf", amount);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
        return;

    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return;

    printf("\n%s\n", res.body);
}

void deposit_money(int sock_fd, Token *user)
{
    Request req;
    Response res;
    double amount;

    printf("Enter the amount you would like to deposit: ");
    scanf("%lf", &amount);

    req.user = *user;
    req.argc = 2;
    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "DEPOSIT %lf", amount);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
        return;

    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return;

    printf("\n%s\n", res.body);
}

void transfer_money(int sock_fd, Token *user)
{
    Request req;
    Response res;
    char ben_username[USERNAME_SIZE];
    double amount;

    printf("\nBeneficiary Username: ");
    scanf("%s", ben_username);

    printf("\nTransfer Amount: ");
    scanf("%lf", &amount);

    req.argc = 3;
    req.user = *user;
    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "TRANSFER %s %lf", ben_username, amount);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0)
        return;
    if (read(sock_fd, &res, sizeof(Response)) < 0)
        return;

    printf("\n%s\n", res.body);
}

void apply_for_loan(int sock_fd, Token *user)
{
    int loan_type;
    double loan_amount;
    Request req;
    Response res;

    printf("\n1. PERSONAL");
    printf("\n2. EDUCATION");
    printf("\n3. BUSINESS");
    printf("\nEnter the type of loan to apply for: ");
    scanf("%d", &loan_type);

    if (loan_type < 1 || loan_type > 3) {
        printf("Invalid choice\n");
        return;
    }

    printf("\nEnter the amount you want to loan: ");
    scanf("%lf", &loan_amount);

    req.user = *user;
    req.argc = 3;

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "LOAN %d %lf", loan_type, loan_amount);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0) return;
    if (read(sock_fd, &res, sizeof(Response)) < 0) return;

    printf("\n%s\n", res.body);
}

void give_feedback(int sock_fd, Token *user)
{
    Request req;
    Response res;
    char feedback[FEEDBACK_SIZE];

    req.user = *user;
    printf("Enter your feedback: \n");
    fgets(feedback, FEEDBACK_SIZE, stdin);
    req.argc = countWords(feedback) + 1;

    snprintf(req.arguments, MAX_ARGUMENT_SIZE - 1, "GIVE_FEEDBACK %s", feedback);

    if (send(sock_fd, &req, sizeof(Request), 0) < 0) return;
    if (read(sock_fd, &res, sizeof(Response)) < 0) return;

    printf("\n%s\n", res.body);
}

void change_password(int sock_fd, Token *user)
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

void customer_handler(int sock_fd, Token *user)
{
    int choice;

    while (1)
    {
        display_customer_menu();
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        clearStdin();

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
            deposit_money(sock_fd, user);
            break;

        case 4:
            transfer_money(sock_fd, user);
            break;

        case 6:
            apply_for_loan(sock_fd, user);
            break;

        case 7:
            give_feedback(sock_fd, user);
            break;

        case 8:
            change_password(sock_fd, user);
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
