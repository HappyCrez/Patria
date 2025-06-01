#include "server.h"
#include "manager.h"

static void print_error_code(int error_code)
{
        switch (error_code)
        {
        case SOCKET_ERROR:
                perror("Socket failed");
                break;
        case OPT_ERROR:
                perror("Configuring socket failed");
                break;
        case BIND_ERROR:
                perror("Bind failed");
                break;
        case LISTEN_ERROR:
                perror("Listen failed");
                break;
        }
}

struct server *server_init(int listen_port)
{
        struct server *server = malloc(sizeof(struct server));
        server->addrlen = sizeof(struct sockaddr_in);
        if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
                print_error_code(SOCKET_ERROR);
        }

        int yes = 1; /* Set option to socket to ignore TIME_WAIT in tcp protocol connection */
        if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
                print_error_code(OPT_ERROR);
        }

        struct sockaddr_in address = server->address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(listen_port);

        /* Bind the socket to the address */
        if (bind(server->server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
                print_error_code(BIND_ERROR);
        }

        /* Listen for incoming connections */
        if (listen(server->server_fd, 3) < 0)
        {
                print_error_code(LISTEN_ERROR);
        }

        if (__OUTPUT_LOGS)
                printf("server: setup success\n");

        server->manager = manager_init(server);
        return server;
}

void server_destroy(server *server)
{
        shutdown(server->server_fd, SHUT_RDWR); /* shutdown all connections */
        close(server->server_fd);               /* close socket file descriptor */

        manager_destroy(server->manager);
        free(server);

        if (__OUTPUT_LOGS)
                printf("server: shutdown\n");
}

void server_start(server *server)
{
        if (__OUTPUT_LOGS)
        {
                printf("server: ready to serve connections\n");
        }
        manager_handle(server->manager);
}