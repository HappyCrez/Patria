/**
 * @file  http.h
 * @brief file include all needed to create any response
 * (that involves connection upgrade to WebSocket)
 */

#pragma once
#include "dependencies.h"
#include "utils/file_tool.h"
#include "utils/base64.h"

/**
 * @brief response
 * @param response is pointer on string with response
 * @param is_ws is flag (is websocket ugrade or no)
 */
struct http_response
{
        struct string *response;
        bool is_ws;
};

/**
 * @brief request types
 * @param UNKNOWN_REQUEST cann't define what request type is
 * @param GET_REQUEST http get type
 * @param POST_REQUEST http post type
 */
enum http_request
{
        UNKNOWN_REQUEST,
        GET_REQUEST,
        POST_REQUEST
};

/* define request type by keyword "POST" */
#define HTTP_POST "POST"

/* define request type by keyword "GET" */
#define HTTP_GET "GET"

/* define request type by keyword "WebSocket" */
#define HTTP_WS "WebSocket"

/**
 * @brief types of get request
 * @param STD_URL it's standart request to get file
 * @param WS_URL is request on upgrade connection
 */
enum http_get_types
{
        STD_URL,
        WS_URL
};

/* http header on success */
#define HTTP_200 "HTTP/1.1 200 OK\n\n"

/* http header on error */
#define HTTP_404 "HTTP/1.1 404 Not Found\n\n"

/**
 * @brief create not found http response
 * @return pointer on new string with response data
 */
struct string *
http_404();

/**
 * @brief create response on get request
 * @param filename file to return
 * @return pointer on new string with response data (200 or 404)
 */
struct string *http_build_response(struct string *filename);

/* length of field key in http upgrade request */
#define WS_KEY_LEN 24

/* field that contain key to make WebSocket upgrade */
#define WS_KEY_FIELD "Sec-WebSocket-Key: "

/* standart sequence that defined in RFC 6455 */
#define GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

/* length of compose guid value and key value */
#define GUIDKEY_LEN 61

/* length of sha1 hash */
#define SHA1_LEN 40

#define HTTP_WS_RESPONSE                     \
        "HTTP/1.1 101 Switching Protocols\n" \
        "Upgrade: websocket\n"               \
        "Connection: Upgrade\n"              \
        "Sec-WebSocket-Accept: "

/**
 * @brief function to create http upgrade response
 * @param request http request data
 * @return response type (get, post or unknown)
 */
enum http_request http_request_type(struct string *request);

/**
 * @brief handle http response
 * @param request http request data
 * @return http response (or null)
 */
struct http_response *http_handle_get(struct string *request);