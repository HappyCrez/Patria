#pragma once
#include "server_fwd.h"

#include "parsers/json_parser.h"
#include "parsers/web_socket_parser.h"

#define WS_HEADER_LEN 95
#define WS_HEADER                            \
        "HTTP/1.1 101 Switching Protocols\n" \
        "Upgrade: websocket\n"               \
        "Connection: Upgrade\n"              \
        "Sec-WebSocket-Accept: "

struct network_manager
{
        pthread_mutex_t mutex;
        server *server;
        struct bst *ws_clients;
        struct ws_parser_callbacks *callbacks;
};

struct web_socket_routine
{
        pthread_mutex_t *mutex;
        struct ws_parser_callbacks *callbacks;
        struct bst *shared_bst;         
        struct pair *client_info;       /* pair{socket,login} */
        pthread_t *pthread;             /* WebSocket handler thread */
};

int network_manager_init(struct network_manager *, struct server *);
void network_manager_destroy(struct network_manager *);
void network_manager_accept_connection(struct network_manager *);