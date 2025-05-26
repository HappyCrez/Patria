#pragma once
#include "Server&NM.h"

#include "Parsers/JSONParser.h"
#include "Parsers/WebSocketParser.h"

#define WS_HEADER_LEN 95
#define WS_HEADER \
    "HTTP/1.1 101 Switching Protocols\n"  \
    "Upgrade: websocket\n"                \
    "Connection: Upgrade\n"               \
    "Sec-WebSocket-Accept: "            

#define STD_ANSWER {0x89, 0x05, 'H', 'e', 'l', 'l', 'o','\0'}

struct NetworkManager
{
    Server *server;
    wsParserCallbacks *callbacks;
    size_t *ws_massive;
    size_t ws_massive_max_size;
    size_t ws_massive_cur_size;
};

typedef struct
{
    NetworkManager *network_manager;
    Pair *client_info;
    pthread_t *pthread_id;
} WebSocketRoutine;

int  InitNetworkManager(NetworkManager*, struct Server*);
void DestroyNetworkManager(NetworkManager*);
void acceptConnection(NetworkManager*);