#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../header/users.h"

#define USER_FILE "users.txt"

void register_user() {
    char username[50];
    char password[50];

    printf("Register a new user\n");

    // Check for the username 'admin'
    while (1) {
        printf("Enter new username: ");
        scanf("%s", username);
        if (strcmp(username, "admin") == 0) {
            printf("Username 'admin' is reserved. Please choose a different username.\n");
        } else {
            break;
        }
    }

    printf("Enter new password: ");
    scanf("%s", password);

    FILE *file = fopen(USER_FILE, "a+");
    if (file == NULL) {
        perror("Error opening file for registration");
        return;
    }

    fprintf(file, "%s %s\n", username, password);
    fclose(file);

    printf("User registered successfully!\n");
}

int login(UserType *userType) {
    char username[50];
    char password[50];
    char file_username[50];
    char file_password[50];
    int user_found = 0;

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    if (strcmp(username, "admin") == 0 && strcmp(password, "admin") == 0) {
        *userType = ADMIN;
        printf("Logged in as admin.\n");
        return 1;
    }

    FILE *file = fopen(USER_FILE, "r+");
    if (file == NULL) {
        perror("Error opening file for login");
        exit(0);
    }

    while (fscanf(file, "%s %s", file_username, file_password) != EOF) {
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            *userType = USER;
            user_found = 1;
            printf("Logged in as user.\n");
            break;
        }
    }

    fclose(file);

    if (!user_found) {
        printf("Invalid username or password. Please try again.\n");
    }

    return user_found;
}
