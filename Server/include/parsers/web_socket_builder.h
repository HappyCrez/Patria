#pragma once
#include "dependencies.h"

enum websocket_flags
{
        // opcodes
        WS_OP_CONTINUE = 0x0,
        WS_OP_TEXT = 0x1,
        WS_OP_BINARY = 0x2,
        WS_OP_CLOSE = 0x8,
        WS_OP_PING = 0x9,
        WS_OP_PONG = 0xA,
        WS_OP_MASK = 0xF,
        
        // marks
        WS_FINAL_FRAME = 0x10,
        WS_HAS_MASK = 0x20,
};

/**
 * Build new WS frame (func allocate memory by self)
 * @flags: ws opcodes and flags (example: WS_OP_TEXT | WS_FINAL_FRAME)
 * @data: data to send in body
 * @data_len: len of data
 * No add mask
 * Return new frame
 */
struct pair *web_socket_build_frame(enum websocket_flags flags, const char *data, size_t data_len);