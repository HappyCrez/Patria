#include "protocols/ws_protocol/ws.h"

static size_t web_socket_calc_frame_size(enum ws_flags flags, size_t data_len)
{
        size_t size = data_len + 2; // body + 2 bytes of head
        if (data_len >= 126)
        {
                if (data_len > 0xFFFF)
                {
                        size += 8;
                }
                else
                {
                        size += 2;
                }
        }
        if (flags & WS_HAS_MASK)
        {
                size += 4;
        }

        return size;
}

struct string *ws_build_frame(enum ws_flags flags, const char *data, size_t data_len)
{
        struct string *frame = string_init(web_socket_calc_frame_size(flags, data_len));

        size_t body_offset = 0;
        frame->data[0] = 0;
        frame->data[1] = 0;
        if (flags & WS_FINAL_FRAME)
        {
                frame->data[0] = (char)(1 << 7);
        }
        
        frame->data[0] |= flags & WS_OP_MASK; /* Add operand code */

        if (data_len < 126)
        {
                frame->data[1] |= data_len;
                body_offset = 2;
        }
        else if (data_len <= 0xFFFF)
        {
                frame->data[1] |= 126;
                frame->data[2] = (char)(data_len >> 8);
                frame->data[3] = (char)(data_len & 0xFF);
                body_offset = 4;
        }
        else
        {
                frame->data[1] |= 127;
                frame->data[2] = (char)((data_len >> 56) & 0xFF);
                frame->data[3] = (char)((data_len >> 48) & 0xFF);
                frame->data[4] = (char)((data_len >> 40) & 0xFF);
                frame->data[5] = (char)((data_len >> 32) & 0xFF);
                frame->data[6] = (char)((data_len >> 24) & 0xFF);
                frame->data[7] = (char)((data_len >> 16) & 0xFF);
                frame->data[8] = (char)((data_len >> 8) & 0xFF);
                frame->data[9] = (char)((data_len) & 0xFF);
                body_offset = 10;
        }
        
        memcpy(&frame->data[body_offset], data, data_len);
        return frame;
}