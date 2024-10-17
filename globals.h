#ifndef GLOBALS_H
#define GLOBALS_H

#define CUSTOMERS_FILE "customers_db"
#define EMPLOYEES_FILE "employees_db"
#define ADMIN_FILE "admin_db"
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 15
#define FEEDBACK_SIZE 300

typedef enum
{
    True,
    False
} Bool;

typedef enum
{
    CUSTOMER = 1,
    EMPLOYEE,
    ADMIN
} UserType;

#endif