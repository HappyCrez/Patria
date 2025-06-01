/**
 * @file  dependencies.h
 * @brief All program dependencies
 */

#pragma once
#include <openssl/sha.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#define null NULL

#define __DEBUG 1
#define __OUTPUT_LOGS 1

/**
 * @brief pair nums
 * @param first one value
 * @param second another value
 */
struct pair
{
        int64_t first;
        int64_t second;
};

/* Max len of filepath in windows */
#define MAX_FILEPATH_SIZE 256

/* path to all clients files */
#define CLIENT_PATH "./Client"

/* main html file */
#define HTML_FILEPATH "./Client/Z"

/* path to DataBase file */
#define DB_FILE "./temp/storage.txt"

/* listen port */
#define SERVER_PORT 8080

/* HTTP specification recommend */
#define MAX_REQUEST_SIZE 8000

/**
 * @brief main erros on server execute
 * @param SUCCESS no errors
 * @param SOCKET_ERROR failed to create socket
 * @param BIND_ERROR failed to bind port (port already in use)
 * @param LISTEN_ERROR failed to open port for listen
 * @param OPT_ERROR port configuring error
 * @param ALLOCATION_ERROR malloc error occures
 */
enum server_error_codes
{
        SUCCESS = 0,
        SOCKET_ERROR,
        BIND_ERROR,
        LISTEN_ERROR,
        OPT_ERROR,
        ALLOCATION_ERROR
};