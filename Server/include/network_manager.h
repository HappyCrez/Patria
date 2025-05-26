#pragma once
#include "server_fwd.h"

#include "parsers/json_parser.h"
#include "parsers/web_socket_parser.h"

#define WS_HEADER_LEN 95
#define WS_HEADER \
    "HTTP/1.1 101 Switching Protocols\n"  \
    "Upgrade: websocket\n"                \
    "Connection: Upgrade\n"               \
    "Sec-WebSocket-Accept: "            

#define STD_ANSWER {0x89, 0x05, 'H', 'e', 'l', 'l', 'o','\0'}

struct network_manager
{
    server *server;
    struct ws_parser_callbacks *callbacks;
    size_t *ws_massive;
    size_t ws_massive_max_size;
    size_t ws_massive_cur_size;
};

struct web_socket_routine
{
    struct network_manager *network_manager;
    struct pair *client_info;
    pthread_t *pthread_id;
};

int  init_network_manager(struct network_manager*, struct server*);
void destroy_network_manager(struct network_manager*);
void accept_connection(struct network_manager*);