#pragma once
// #include <openssl/ssl.h> // try to set ssl connection
// #include <openssl/err.h>
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

// Base definitions
typedef long long ll;
typedef char bool;
typedef char byte;

#define __DEBUG 1
#define __OUTPUT_LOGS 1

#define TRUE 1
#define FALSE 0

#define MEMORY_ERROR 1

struct pair
{
        ll first;
        ll second;
};

struct server_response
{
        struct pair *data;
        bool isWS;
};

// Pages
#define MAX_FILEPATH_SIZE 1000 // The most depeer files is more shorter than 1000 symbols
#define PATH_TO_CLIENT_FILES_LEN 9
#define PATH_TO_CLIENT_FILES "./Client"
#define PATH_TO_SRC_FILE_LEN 11
#define PATH_TO_SRC_FILE "./Client/Z"
#define PATH_TO_STORAGE "./temp/storage.txt"

// TCP connection
#define SERVER_PORT 8080
#define MAX_REQUEST_SIZE 8000 // HTTP specification recommend

enum connection_error
{
        SUCCESS = 0,
        SOCKET_ERROR,
        BIND_ERROR,
        LISTEN_ERROR,
        OPT_ERROR
};

enum http_request
{
        UNKNOWN_REQUEST,
        GET_REQUEST,
        POST_REQUEST
};

// GET
#define HTTP_GET "GET"
#define HTTP_WS "WebSocket"

enum http_get_types
{
        STD_URL,
        WEB_SOCKET_URL
};

// POST types
#define HTTP_POST "POST"
#define HTTP_POST_LOGIN "login"
#define HTTP_POST_RECV_MESSAGES "recv_messages"
#define HTTP_POST_RECV_DIALOGS "recv_dialogs"

enum http_post_types
{
        UNKNOWN_POST,
        LOGIN_POST,
        RECIVE_MESSAGES_POST,
        RECIVE_DIALOGS_POST
};

// WEB SOCKET (WS)
#define WS_KEY_LEN 24
#define WS_KEY_FIELD_LEN 20
#define WS_KEY_FIELD "Sec-WebSocket-Key: "

#define GUID_LEN 37
#define GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define GUIDKEY_LEN 61

#define SHA1_LEN 41
#define BASE64_LEN 56

enum ws_frame_type_t
{
        WS_FRAME_TEXT = 0x1,
        WS_FRAME_BINARY = 0x2,
        WS_FRAME_CLOSE = 0x8,
        WS_FRAME_PING = 0x9,
        WS_FRAME_PONG = 0xA,
};

enum ws_parser_error_codes
{
        WS_OK,
        WS_RESERVED_BITS_SET,
        WS_INVALID_OPCODE,
        WS_INVALID_CONTINUATION,
        WS_CONTROL_TOO_LONG,
        WS_NON_CANONICAL_LENGTH,
        WS_FRAGMENTED_CONTROL
};