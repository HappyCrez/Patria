#include "parsers/web_socket_builder.h"

static size_t web_socket_calc_frame_size(enum websocket_flags flags, size_t data_len)
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

struct pair *web_socket_build_frame(enum websocket_flags flags, const char *data, size_t data_len)
{
        struct pair *frame = malloc(sizeof(struct pair));
        frame->second = web_socket_calc_frame_size(flags, data_len);
        frame->first = (ll)malloc(frame->second);

        size_t body_offset = 0;
        ((char*)frame->first)[0] = 0;
        ((char*)frame->first)[1] = 0;
        if (flags & WS_FINAL_FRAME)
        {
                ((char*)frame->first)[0] = (char)(1 << 7);
        }
        
        ((char*)frame->first)[0] |= flags & WS_OP_MASK; /* Add operand code */

        if (data_len < 126)
        {
                ((char*)frame->first)[1] |= data_len;
                body_offset = 2;
        }
        else if (data_len <= 0xFFFF)
        {
                ((char*)frame->first)[1] |= 126;
                ((char*)frame->first)[2] = (char)(data_len >> 8);
                ((char*)frame->first)[3] = (char)(data_len & 0xFF);
                body_offset = 4;
        }
        else
        {
                ((char*)frame->first)[1] |= 127;
                ((char*)frame->first)[2] = (char)((data_len >> 56) & 0xFF);
                ((char*)frame->first)[3] = (char)((data_len >> 48) & 0xFF);
                ((char*)frame->first)[4] = (char)((data_len >> 40) & 0xFF);
                ((char*)frame->first)[5] = (char)((data_len >> 32) & 0xFF);
                ((char*)frame->first)[6] = (char)((data_len >> 24) & 0xFF);
                ((char*)frame->first)[7] = (char)((data_len >> 16) & 0xFF);
                ((char*)frame->first)[8] = (char)((data_len >> 8) & 0xFF);
                ((char*)frame->first)[9] = (char)((data_len) & 0xFF);
                body_offset = 10;
        }
        
        memcpy(&((char*)frame->first)[body_offset], data, data_len);
        return frame;
}