#ifndef UDP_LISTENER_H
#define UDP_LISTENER_H


typedef struct {
    int port;     
    int verbose;  
} udp_args_t;


void *udp_listener(void *arg);

#endif
