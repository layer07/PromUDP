#include "http_server.h"
#include "metrics.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define BUFFER_SIZE 16384 


typedef struct {
    int client_socket;
    int verbose;
} http_thread_args_t;


void send_full_response(int socket, const char *response, size_t response_length) {
    size_t total_sent = 0;
    while (total_sent < response_length) {
        ssize_t sent = send(socket, response + total_sent, response_length - total_sent, 0);
        if (sent < 0) {
            perror("Failed to send response");
            break;
        }
        total_sent += sent;
    }
}

void *handle_client(void *arg) {
    http_thread_args_t *client_args = (http_thread_args_t *)arg;
    int client_socket = client_args->client_socket;
    int verbose = client_args->verbose;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    free(client_args);

    
    getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_len);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr.sin_port);

   
    char *body = malloc(BUFFER_SIZE);
    if (!body) {
        perror("Failed to allocate memory for metrics body");
        close(client_socket);
        return NULL;
    }
    format_metrics(body, BUFFER_SIZE);
    int content_length = strlen(body);

    
    size_t response_size = content_length + 128;  // Estimate headers size
    char *response = malloc(response_size);
    if (!response) {
        perror("Failed to allocate memory for HTTP response");
        free(body);
        close(client_socket);
        return NULL;
    }

    
    snprintf(response, response_size,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             content_length, body);

    if (verbose) {
        printf("Client connected: %s:%d\n", client_ip, client_port);
        printf("Response headers and body:\n%s\n", response);
    }

    
    send_full_response(client_socket, response, strlen(response));

    
    shutdown(client_socket, SHUT_WR); 
    if (verbose) {
        printf("Shutdown write connection for client: %s:%d\n", client_ip, client_port);
    }

    close(client_socket); 
    if (verbose) {
        printf("Client disconnected: %s:%d\n", client_ip, client_port);
    }

    free(body);
    free(response);
    return NULL;
}





void *http_server(void *arg) {
    server_args_t *args = (server_args_t *)arg; 
    int http_port = args->port;
    int verbose = args->verbose;
    int server_socket, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

   
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("HTTP socket creation failed");
        pthread_exit(NULL);
    }

    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(http_port);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("HTTP socket bind failed");
        close(server_socket);
        pthread_exit(NULL);
    }

    if (listen(server_socket, 10) < 0) {
        perror("HTTP listen failed");
        close(server_socket);
        pthread_exit(NULL);
    }

    printf("HTTP Server running on 0.0.0.0:%d\n", http_port);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_socket < 0) {
            perror("HTTP accept failed");
            continue;
        }

        
        http_thread_args_t *client_args = malloc(sizeof(http_thread_args_t));
        if (client_args == NULL) {
            perror("Failed to allocate memory for client thread arguments");
            close(client_socket);
            continue;
        }

        client_args->client_socket = client_socket;
        client_args->verbose = verbose;

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, client_args) != 0) {
            perror("Failed to create client thread");
            free(client_args);
            close(client_socket);
            continue;
        }

        pthread_detach(client_thread);  // Detach thread to avoid blocking
    }

    close(server_socket);
    pthread_exit(NULL);
}
