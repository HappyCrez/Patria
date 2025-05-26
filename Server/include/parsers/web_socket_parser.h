#pragma once
#include "dependencies.h"

struct ws_parser_callbacks {
    int(*on_data_begin)     (void*, enum ws_frame_type_t);
    int(*on_data_payload)   (void*, const char*, size_t);
    int(*on_data_end)       (void*);
    int(*on_control_begin)  (void*, enum ws_frame_type_t);
    int(*on_control_payload)(void*, const char*, size_t);
    int(*on_control_end)    (void*);
};

struct ws_parser{
    uint64_t bytes_remaining;
    uint8_t mask[4];
    uint8_t fragment  : 1;
    uint8_t fin       : 1;
    uint8_t control   : 1;
    uint8_t mask_flag : 1;
    uint8_t mask_pos  : 2;
    uint8_t state     : 5;
};

void ws_parser_init(struct ws_parser* parser);

int ws_parser_execute(
    struct ws_parser* parser,
    const struct ws_parser_callbacks* callbacks,
    void* data,
    char* buff /* mutates! */,
    size_t len);

const char* ws_parser_error(int rc);