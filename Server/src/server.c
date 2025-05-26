#include "server.h"
#include "network_manager.h"

int init_server(server *server, int listen_port)
{
    server->addrlen = sizeof(struct sockaddr_in);
    if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        return SOCKET_ERROR;
    }

    int yes = 1; // Set option to socket to ignore TIME_WAIT in tcp protocol connection
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        return OPT_ERROR;
    }

    struct sockaddr_in address = server->address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(listen_port);

    // Bind the socket to the address
    if (bind(server->server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return BIND_ERROR;
    }

    // Listen for incoming connections
    if (listen(server->server_fd, 3) < 0) {
        return LISTEN_ERROR;
    }
    
    if (__OUTPUT_LOGS)
    {
        printf("server: setup success\n");
    }
    
    server->network_manager = malloc(sizeof(network_manager));
    return init_network_manager(server->network_manager, server);
}

void destroy_server(server *server)
{
    shutdown(server->server_fd, SHUT_RDWR); // ShutDown all connections 
    close(server->server_fd);               // Close socket file descriptor

    destroy_network_manager(server->network_manager);
    free(server);

    if (__OUTPUT_LOGS)
    {
        printf("server: shutdown\n");
    }
}

void start_server(server *server)
{
    if (__OUTPUT_LOGS)
    {
        printf("server: ready to serve connections\n");
    }
    accept_connection(server->network_manager);
}