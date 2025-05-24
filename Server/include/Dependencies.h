#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
// #include "microhttpd.h"
// #include <openssl/ssl.h>
// #include <openssl/err.h>

#define _DEBUG 0

// Base definitions
typedef long long ll;
typedef char bool;
#define TRUE 1
#define FALSE 0

struct pair
{
    ll first;
    ll second;
};

// JSON Data "field_name":"field_val"
struct parsedData
{
    struct pair field_name;
    struct pair field_val;
};

// Pages
#define MAX_FILEPATH_SIZE 1000 // The most depeer files is more shorter than 1000 symbols
#define PATH_TO_CLIENT_FILES "./Client\0"
#define PATH_TO_STORAGE "./temp/storage.txt\0"

// TCP connection
#define SERVER_PORT 8080
#define MAX_REQUEST_SIZE 8000  // HTTP specification recommend

typedef enum
{
    SUCCESS = 0,
    SOCKET_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
    OPT_ERROR
} CONNECTION_ERROR;

typedef enum
{
    UNKNOWN_REQUEST,
    GET_REQUEST,
    POST_REQUEST
} HTTP_REQUEST;

// GET
#define HTTP_GET  "GET\0"

// POST types
#define HTTP_POST "POST\0"
#define HTTP_POST_LOGIN "login\0"
#define HTTP_POST_RECV_MESSAGES "recv_messages\0"
#define HTTP_POST_RECV_DIALOGS "recv_dialogs\0"

typedef enum
{
    UNKNOWN_POST,
    LOGIN_POST,
    RECIVE_MESSAGES_POST,
    RECIVE_DIALOGS_POST
} HTTP_POST_TYPES;