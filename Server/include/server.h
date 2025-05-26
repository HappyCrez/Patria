#pragma once
#include "dependencies.h"
#include "network_manager_fwd.h"

struct server
{
    int server_fd;
    int new_socket;
    struct sockaddr_in address;
    int addrlen;
    network_manager *network_manager;
};

int  init_server(struct server*, int listen_port);
void destroy_server(struct server*);
void start_server(struct server*);