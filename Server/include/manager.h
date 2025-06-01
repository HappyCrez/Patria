/**
 * @file manager.h
 * @brief manager used to serve connection for current server 
 */

#pragma once
#include "server_fwd.h"
#include "utils/bst.h"
#include "protocols/json.h"
#include "protocols/http.h"
#include "protocols/ws_protocol/ws_handler.h"

/**
 * @brief manager has all data about clients
 * @param mutex used to manage access to shared data (ws_clients)
 * @param server is pointer on server where manager working
 * @param ws_clients is binary search tree of all connected users
 * @param callbacks is list of functions to serve WebSocket connections
 */
struct manager
{
        pthread_mutex_t mutex;
        struct server *server;
        struct bst *ws_clients;
        struct ws_parser_callbacks *callbacks;
};

/**
 * @brief struct define data for any connection (separate thread)
 * @param mutex manage access to shared data
 * @param callbacks functions to serve WebSocket requests
 * @param shared_bst shared data of all connections
 * @param client_info info about connection
 * @param pthread id of thread where connection is serving
 */
struct ws_routine
{
        pthread_mutex_t *mutex;
        struct ws_parser_callbacks *callbacks;
        struct bst *shared_bst;         
        struct pair *client_info;       /* pair{login, socket} */
        pthread_t *pthread;             /* WebSocket handler thread */
};

/**
 * @brief create new manager of connections
 * @param server is pointer on server there manager will function
 * @return pointer on new manager 
 */
struct manager *manager_init(struct server *server);

/**
 * @brief correct destroy manager
 * @param manager pointer on manager to destroy
 */
void manager_destroy(struct manager *manager);

/**
 * @brief connections handler (infinity loop)
 * @param manager pointer on manager to serve handle connections 
 */
void manager_handle(struct manager *manager);