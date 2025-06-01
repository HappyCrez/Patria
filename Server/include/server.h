/**
 * @file  server.h
 * @brief main server information
 */

#pragma once
#include "dependencies.h"
#include "manager_fwd.h"

/**
 * @brief contain main data to serve connections
 */
struct server
{
        int server_fd;
        int new_socket;
        struct sockaddr_in address;
        int addrlen;
        struct manager *manager;
};

/**
 * @brief initialize server
 * @param listen_port is define port that server will use
 * @return pointer on new server
 */
struct server *server_init(int listen_port);

/**
 * @brief destroy server
 * @param server pointer on server to destroy 
 */
void server_destroy(struct server *server);

/**
 * @brief start to serve connections
 * @param server pointer on server that will intercept thread
 * @note it will handle connections in infinity loop
 */
void server_start(struct server *server);