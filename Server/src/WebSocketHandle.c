#include "WebSocketHandle.h"
#include "Parsers/JSONParser.h"

int onDataPayload(void* user_data, const char* buff, size_t len)
{
    printf("%s\n\n", buff);
    char *request = buff;
    ++request; // Skip --> {
    JSONData *dataType = parseJSONRow(&request);

    JSONData *reciver = parseJSONRow(&request);
    JSONData *message = parseJSONRow(&request);


    printf("'%s':'%s'\n", (char*)dataType->field_name->first, (char*)dataType->field_val->first);
    printf("'%s':'%s'\n", (char*)reciver->field_name->first , (char*)reciver->field_val->first);
    printf("'%s':'%s'\n", (char*)message->field_name->first , (char*)message->field_val->first);

    free(dataType);
    free(reciver);
    free(message);
    return WS_OK;
}

int onControlBegin(void* user_data, ws_frame_type_t frame_type)
{
    (void)user_data;

    printf("control_begin: %s\n",
        frame_type == WS_FRAME_PING  ? "ping" :
        frame_type == WS_FRAME_PONG  ? "pong" :
        frame_type == WS_FRAME_CLOSE ? "close" :
        "?");

    return WS_OK;
}

int onControlPayload(void* user_data, const char* buff, size_t len)
{
    (void)user_data;

    printf("control_payload: '%.*s'\n", (int)len, buff);

    return WS_OK;
}