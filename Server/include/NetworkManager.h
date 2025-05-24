#pragma once

struct NetworkManager
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen;
};

int InitNetworkManager(struct NetworkManager*, int);
void DestroyNetworkManager(struct NetworkManager*);
void serveConnection(struct NetworkManager*);