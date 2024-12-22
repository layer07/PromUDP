#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "udp_listener.h"
#include "http_server.h"

int main(int argc, char *argv[]) {
    int http_port = 8080;  // Default HTTP port
    int udp_port = 9999;  // Default UDP port
    int verbose = 0;      // Default verbosity: off

    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            http_port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--udp") == 0 && i + 1 < argc) {
            udp_port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else {
            fprintf(stderr, "Usage: %s [--port <port>] [--udp <udp_port>] [--verbose]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    pthread_t udp_thread, http_thread;

   
    server_args_t server_args = {http_port, verbose};

    
    udp_args_t udp_args = {udp_port, verbose};

    // Create the UDP listener thread
    if (pthread_create(&udp_thread, NULL, udp_listener, &udp_args) != 0) {
        perror("Failed to create UDP listener thread");
        exit(EXIT_FAILURE);
    }

    // Create the HTTP server thread
    if (pthread_create(&http_thread, NULL, http_server, &server_args) != 0) {
        perror("Failed to create HTTP server thread");
        exit(EXIT_FAILURE);
    }

    
    pthread_join(udp_thread, NULL);
    pthread_join(http_thread, NULL);

    return 0;
}
