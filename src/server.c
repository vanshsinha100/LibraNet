#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../header/books.h"
#include "../header/users.h"

#define PORT 8080
#define BUFFER_SIZE 1024

void send_response(int client_socket, const char *response) {
    write(client_socket, response, strlen(response));
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    char isbn[20];
    char title[100];
    char author[100];
    int copies;
    char response[BUFFER_SIZE];

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        read(client_socket, buffer, BUFFER_SIZE);

        // Extract the command
        sscanf(buffer, "%s", command);

        if (strncmp("add", command, 3) == 0) {
            // Extract parameters for add
            sscanf(buffer + 4, "%19s %99[^\t] %99[^\t] %d", isbn, title, author, &copies);
            add_book(isbn, title, author, copies, response);
        } else if (strncmp("rent", command, 4) == 0) {
            // Extract parameters for rent
            sscanf(buffer + 5, "%19s", isbn);
            rent_book(isbn, response);
        } else if (strncmp("delete", command, 6) == 0) {
            // Extract parameters for delete
            sscanf(buffer + 7, "%19s", isbn);
            delete_book(isbn, response);
        } else if (strncmp("modify", command, 6) == 0) {
            // Extract parameters for modify
            sscanf(buffer + 7, "%19s %99[^\t] %99[^\t] %d", isbn, title, author, &copies);
            modify_book(isbn, title, author, copies, response);
        } else if (strncmp("search", command, 6) == 0) {
            // Extract parameters for search
            sscanf(buffer + 7, "%19s", isbn);
            search_book(isbn, response);
        } else if (strncmp("logout", command, 6) == 0) {
            printf("Client logged out.\n");
            snprintf(response, BUFFER_SIZE, "Logged out.\n");
            send_response(client_socket, response);
            break;
        } else {
            snprintf(response, BUFFER_SIZE, "Invalid command.\n");
        }

        send_response(client_socket, response);
    }
    close(client_socket);
}

void *client_thread(void *arg) {
    int client_socket = *((int *)arg);
    free(arg);
    handle_client(client_socket);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) != 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Server accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Client connected\n");

        pthread_t thread;
        int *client_sock = malloc(sizeof(int));
        *client_sock = client_socket;
        pthread_create(&thread, NULL, client_thread, client_sock);
        pthread_detach(thread); // Detach the thread to avoid memory leaks
    }

    close(server_socket);
    return 0;
}
