/**
 * @file  ws.h
 * @brief file describe all need data to use WebSocket protocol
 */

#pragma once
#include "dependencies.h"
#include "utils/string.h"

/**
 * @brief possible types of WebSocket frame and marks to build frame
 * @param WS_OP_CONTINUE opcode that frame is fragmented
 * @param WS_OP_TEXT opcode of text data in frame
 * @param WS_OP_BINARY opcode of binary data in frame
 * @param WS_OP_CLOSE opcode of close connection
 * @param WS_OP_PING opcode of request on check connection
 * @param WS_OP_PONG opcode of response on check connection
 * @param WS_OP_MASK opcode of masked data in frame
 *
 * @param WS_FINAL_FRAME mark to build final frame
 * @param WS_HAS_MASK mark to set mask on frame (not realized in builder)
 */
enum ws_flags
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
 * @brief struct to contain all callbacks on parse event
 * @param on_data_begin
 * @param on_data_payload
 * @param on_data_end
 * @param on_control_begin
 * @param on_control_payload
 * @param on_control_end
 */
struct ws_parser_callbacks
{
        int (*on_data_begin)(void *, enum ws_flags);
        int (*on_data_payload)(void *, char *, size_t);
        int (*on_data_end)(void *);
        int (*on_control_begin)(void *, enum ws_flags);
        int (*on_control_payload)(void *, const char *, size_t);
        int (*on_control_end)(void *);
};

/**
 * @brief struct to use in WebSocket parse functions
 * @note all fields are private, except callbacks
 */
struct ws_parser
{
        struct ws_parser_callbacks *callbacks;
        uint64_t bytes_remaining;
        uint8_t mask[4];
        uint8_t fragment : 1;
        uint8_t fin : 1;
        uint8_t control : 1;
        uint8_t mask_flag : 1;
        uint8_t mask_pos : 2;
        uint8_t state : 5;
};

/**
 * @brief errors that can occures on parsing WebSocket frame
 * @param WS_OK no erros (Success)
 * @param WS_RESERVED_BITS_SET set any of rcv1,2,3 bits
 * @param WS_INVALID_OPCODE frame opcode is not correct
 * @param WS_INVALID_CONTINUATION recived frame passed after final 
 * @param WS_CONTROL_TOO_LONG len of data in control frame <= 126 bytes 
 * @param WS_NON_CANONICAL_LENGTH len fields isn't correct filled
 * @param WS_FRAGMENTED_CONTROL control frame can't be fragmented
 * @param WS_INVALID_DATA all WebSocket frames should transmit json data
 * (programs axiom) 
 */
enum ws_parser_error_codes
{
        WS_OK,
        WS_RESERVED_BITS_SET,
        WS_INVALID_OPCODE,
        WS_INVALID_CONTINUATION,
        WS_CONTROL_TOO_LONG,
        WS_NON_CANONICAL_LENGTH,
        WS_FRAGMENTED_CONTROL,
        WS_INVALID_DATA
};

/**
 * @brief create new parser
 * @param callbacks pointer on struct that functions will be executed on parsing
 * @return pointer on new parser
 */
struct ws_parser *ws_parser_init(struct ws_parser_callbacks *callbacks);

/**
 * @brief execute parsing on passed data
 * @param parser pointer on parser with callbacks
 * @param data pointer on data that will be pass into callbacks
 * @param buff WebSocket frame to parse
 * @param len is length of frame (without '\0')
 * @return parsing code error (usually expects WS_OK)
 */
int ws_parser_execute(struct ws_parser *parser,
                      void *data,
                      char *buff /* mutates! */,
                      size_t len);

/**
 * @brief function to print in stdout value of error code
 * @param rc is return code
 * @return pointer on string that has error name
 */
const char *ws_parser_error(int rc);

/**
 * @brief Build new WS frame (func allocate memory by self)
 * @param flags ws opcodes and flags (example: WS_OP_TEXT | WS_FINAL_FRAME)
 * @param data data to send in body
 * @param data_len len of data
 * @return new WebSocket frame as string
 * @note builder is not add mask
 */
struct string *ws_build_frame(enum ws_flags flags, const char *data, size_t data_len);