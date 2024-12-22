#include "udp_listener.h"
#include "metrics.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

void *udp_listener(void *arg) {
    udp_args_t *args = (udp_args_t *)arg;
    int udp_port = args->port;
    int verbose = args->verbose;

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

   
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("UDP socket creation failed");
        pthread_exit(NULL);
    }

    // Bind the socket to 0.0.0.0:<udp_port>
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(udp_port);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("UDP socket bind failed");
        close(sockfd);
        pthread_exit(NULL);
    }    
     printf("UDP Listener running on 0.0.0.0:%d\n", udp_port);
    while (1) {
        // Receive data
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("UDP recvfrom failed");
            continue;
        }
        buffer[n] = '\0';

        if (verbose) {
            printf("Received packet: %s\n", buffer);
        }

        // Parse the message
        char name[50], tag[100];
        double value;
        time_t timestamp;
        if (sscanf(buffer, "%49[^,],%99[^,],%lf,%ld", name, tag, &value, &timestamp) == 4) {
            if (verbose) {
                printf("Parsed metric - Name: %s, Tag: %s, Value: %.2f, Timestamp: %ld\n",
                       name, tag, value, timestamp);
            }
            add_or_update_metric(name, tag, value, timestamp);
        } else if (verbose) {
            fprintf(stderr, "Invalid message format: %s\n", buffer);
        }
    }

    close(sockfd);
    pthread_exit(NULL);
}
