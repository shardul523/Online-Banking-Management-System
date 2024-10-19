#include "customer_client.c"
#include "employee_client.c"
#include "admin_client.c"

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

    if (login(socket_fd, user, username, password, (UserType)user_type) < 0)
        return -1;
    
    if (user->user_id == -1) return -1;

    switch (user_type)
    {
        case CUSTOMER:
            customer_handler(socket_fd, user);
            break;

        case EMPLOYEE:
            employee_handler(socket_fd, user);
            break;

        case ADMIN:
            admin_handler(socket_fd, user);
            break;
        
        default:
            break;
    }

    // switch (user_type)
    // {
    // case CUSTOMER:
    //     if (login(socket_fd, user, username, password, CUSTOMER) < 0)
    //         break;
    //     if (user->user_id != -1)
    //         customer_handler(socket_fd, user);
    //     break;
    // case EMPLOYEE:
    //     if (login(socket_fd, user, username, password, EMPLOYEE) < 0)
    //         break;
    //     if (user->user_id)
    //     display_employee_menu();
    //     break;
    // case ADMIN:
    //     login(socket_fd, user, username, password, ADMIN);
    //     display_admin_menu();
    //     break;
    // default:
    //     break;
    // }

    close(socket_fd);
    return 0;
}
