#pragma once
#include "Dependencies.h"
#include "Server&NM.h"

struct Server
{
    int server_fd;
    int new_socket;
    struct sockaddr_in address;
    int addrlen;
    NetworkManager *network_manager;
};

int  InitServer(Server*, int listen_port);
void DestroyServer(Server*);
void StartServer(Server*);