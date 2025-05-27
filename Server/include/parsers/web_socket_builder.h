#pragma once
#include "dependencies.h"

/**
 * Build new WS frame (func allocate memory by self)
 * @flags: ws opcodes and flags (example: WS_OP_TEXT | WS_FINAL_FRAME)
 * @data: data to send in body
 * @data_len: len of data
 * No add mask
 * Return new frame
 */
struct pair *web_socket_build_frame(enum websocket_flags flags, const char *data, size_t data_len);