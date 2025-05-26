#pragma once
#include "Dependencies.h"

typedef struct {
    int(*onDataBegin)     (void*, ws_frame_type_t);
    int(*onDataPayload)   (void*, const char*, size_t);
    int(*onDataEnd)       (void*);
    int(*onControlBegin)  (void*, ws_frame_type_t);
    int(*onControlPayload)(void*, const char*, size_t);
    int(*onControlEnd)    (void*);
} wsParserCallbacks;

typedef struct {
    uint64_t bytes_remaining;
    uint8_t mask[4];
    uint8_t fragment  : 1;
    uint8_t fin       : 1;
    uint8_t control   : 1;
    uint8_t mask_flag : 1;
    uint8_t mask_pos  : 2;
    uint8_t state     : 5;
} wsParser;

void ws_parser_init(wsParser* parser);

int ws_parser_execute(
    wsParser* parser,
    const wsParserCallbacks* callbacks,
    void* data,
    char* buff /* mutates! */,
    size_t len);

const char* ws_parser_error(int rc);