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

int server_init(struct server *, int listen_port);
void server_destroy(struct server *);
void server_start(struct server *);