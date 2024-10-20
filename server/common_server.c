#ifndef COMMON_SERVER_H
#define COMMON_SERVER_H

#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "../globals.h"

void logout(Response *res)
{
    res->user.user_id = -1;
    res->user.user_type = NONE;
    strcpy(res->user.username, "");
    strcpy(res->body, "Logout successful\n");
}

void extract_args(int c, char **a, char *str)
{
    char delim[] = " ";
    char *saveptr;

    a[0] = __strtok_r(str, delim, &saveptr);

    for (int i = 1; i < c; i++)
    {
        a[i] = __strtok_r(NULL, delim, &saveptr);
    }
}

Record *get_record()
{
    int record_fd = open(RECORDS_FILE, O_RDONLY);

    if (record_fd == -1)
        return NULL;

    Record *rec = (Record *)malloc(sizeof(Record));

    if (read(record_fd, rec, sizeof(Record)) < 0)
        return NULL;

    return rec;
}

Bool update_record(int cust_c, int emp_c, int adm_c)
{
    Record rec = {cust_c, emp_c, adm_c};

    int record_fd = open(RECORDS_FILE, O_WRONLY);

    if (record_fd == -1)
        return False;

    if (write(record_fd, &rec, sizeof(Record)) < 0)
        return False;

    return True;
}

Bool are_equal(char *str1, char *str2)
{
    return strcmp(str1, str2) == 0;
}

void print_customer(Customer *cust)
{
    printf("\nCustomer ID: %d\n", cust->customer_id);
    printf("Customer Username: %s\n", cust->username);
    printf("Is Customer Active: %d\n", cust->is_active);
}

#endif