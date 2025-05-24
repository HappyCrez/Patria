#pragma once
#include "Dependencies.h"

#define HTTP_RESPONSE_HEADER_SIZE 142
#define HTTP_RESPONSE_HEADER \
    "HTTP/1.1 200 OK\n" \
    "Vary: Origin\n" \
    "Access-Control-Allow-Credentials: true\n" \
    "Accept-Ranges: bytes\n" \
    "Cache-Control: public, max-age=0\n" \
    "Connection: close\n\n"
    // "Keep-Alive: timeout=0\n\n"

// Return actual HTTP header size OR -1
struct pair createHTTPResponse(char* filename);