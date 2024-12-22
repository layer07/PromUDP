#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H


typedef struct {
    int port;
    int verbose;
} server_args_t;


void *http_server(void *arg);

#endif
