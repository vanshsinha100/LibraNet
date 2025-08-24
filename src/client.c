#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../header/users.h"

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char command[BUFFER_SIZE];
} ThreadArgs;

void send_command(int socket, const char *command) {
    write(socket, command, strlen(command));
}

void receive_response(int socket) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    read(socket, buffer, BUFFER_SIZE);
    printf("%s", buffer);
}

void *perform_operation(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    send_command(threadArgs->socket, threadArgs->command);
    receive_response(threadArgs->socket);
    free(threadArgs); // Free the allocated memory for the thread arguments
    return NULL;
}

void admin_menu(int socket) {
    int choice;
    char isbn[20];
    char title[100];
    char author[100];
    int copies;
    pthread_t thread;

    do {
        printf("\nLibrary Management System - Admin Menu\n");
        printf("1. Add Book\n");
        printf("2. Rent Book\n");
        printf("3. Delete Book\n");
        printf("4. Modify Book\n");
        printf("5. Search Book\n");
        printf("6. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        args->socket = socket;

        switch (choice) {
            case 1:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                printf("Enter Title: ");
                scanf(" %[^\n]%*c", title);
                printf("Enter Author: ");
                scanf(" %[^\n]%*c", author);
                printf("Enter Number of Copies: ");
                scanf("%d", &copies);
                snprintf(args->command, BUFFER_SIZE, "add %s\t%s\t%s\t%d", isbn, title, author, copies);
                pthread_create(&thread, NULL, perform_operation, (void *)args);
                pthread_join(thread, NULL);
                break;
            case 2:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(args->command, BUFFER_SIZE, "rent %s", isbn);
                pthread_create(&thread, NULL, perform_operation, (void *)args);
                pthread_join(thread, NULL);
                break;
            case 3:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(args->command, BUFFER_SIZE, "delete %s", isbn);
                pthread_create(&thread, NULL, perform_operation, (void *)args);
                pthread_join(thread, NULL);
                break;
            case 4:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                printf("Enter New Title: ");
                scanf(" %[^\n]%*c", title);
                printf("Enter New Author: ");
                scanf(" %[^\n]%*c", author);
                printf("Enter New Number of Copies: ");
                scanf("%d", &copies);
                snprintf(args->command, BUFFER_SIZE, "modify %s\t%s\t%s\t%d", isbn, title, author, copies);
                pthread_create(&thread, NULL, perform_operation, (void *)args);
                pthread_join(thread, NULL);
                break;
            case 5:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(args->command, BUFFER_SIZE, "search %s", isbn);
                pthread_create(&thread, NULL, perform_operation, (void *)args);
                pthread_join(thread, NULL);
                break;
            case 6:
                snprintf(args->command, BUFFER_SIZE, "logout");
                pthread_create(&thread, NULL, perform_operation, (void *)args);
                pthread_join(thread, NULL);
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
        // Removed free(args) as it is already freed in perform_operation()
    } while (choice != 6);
}

void user_menu(int socket) {
    int choice;
    char isbn[20];
    pthread_t thread;

    do {
        printf("\nLibrary Management System - User Menu\n");
        printf("1. Rent Book\n");
        printf("2. Search Book\n");
        printf("3. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        args->socket = socket;

        switch (choice) {
            case 1:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(args->command, BUFFER_SIZE, "rent %s", isbn);
                pthread_create(&thread, NULL, perform_operation, (void *)args);
                pthread_join(thread, NULL);
                break;
            case 2:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(args->command, BUFFER_SIZE, "search %s", isbn);
                pthread_create(&thread, NULL, perform_operation, (void *)args);
                pthread_join(thread, NULL);
                break;
            case 3:
                snprintf(args->command, BUFFER_SIZE, "logout");
                pthread_create(&thread, NULL, perform_operation, (void *)args);
                pthread_join(thread, NULL);
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
        // Removed free(args) as it is already freed in perform_operation()
    } while (choice != 3);
}

void user_options(int socket) {
    int choice;
    UserType userType;
    int login_success = 0;

    do {
        printf("\n1. Register as new user\n");
        printf("2. Login as existing user\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
            printf("Invalid choice. Please enter 1 to register or 2 to login.\n");
            while (getchar() != '\n'); // Clear invalid input from buffer
            continue;
        }

        if (choice == 1) {
            register_user();
            printf("Please login with your new credentials.\n");
        }

        login_success = login(&userType);
    } while (!login_success);

    if (userType == ADMIN) {
        admin_menu(socket);
    } else {
        user_menu(socket);
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        perror("Connection to the server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    user_options(client_socket);

    close(client_socket);
    return 0;
}
