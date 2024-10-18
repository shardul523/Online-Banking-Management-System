#ifndef GLOBALS_H
#define GLOBALS_H

#define CUSTOMERS_FILE "data/CUSTOMERS_DB"
#define EMPLOYEES_FILE "EMPLOYEES_DB"
#define ADMIN_FILE "admin_db"
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 15
#define FEEDBACK_SIZE 300
#define RES_BODY_SIZE 1024
#define MAX_ARGUMENT_SIZE 100
#define SERVER_PORT 8080

typedef enum
{
    False,
    True
} Bool;

typedef enum
{
    NONE,
    CUSTOMER,
    EMPLOYEE,
    ADMIN
} UserType;

typedef struct
{
    int user_id;
    UserType user_type;
} Token;

typedef struct
{
    int argc;
    char arguments[MAX_ARGUMENT_SIZE];
    Token user;
} Request;

typedef struct
{
    Token user;
    char body[RES_BODY_SIZE];
} Response;

typedef struct
{
    int customer_id;
    double balance;
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    // char feedback[FEEDBACK_SIZE];
} Customer;

typedef enum
{
    REGULAR,
    MANAGER
} Role;

typedef struct
{
    Role role;
    int employee_id;
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} Employee;

typedef struct
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} Admin;

#endif