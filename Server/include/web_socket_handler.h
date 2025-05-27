#pragma once
#include "dependencies.h"

#define WS_LOGIN_LEN 6
#define WS_LOGIN "login"
#define WS_SEND_MESSAGE_LEN 13
#define WS_SEND_MESSAGE "send_message"
#define WS_SEARCH_LEN 13
#define WS_SEARCH "search_login"

// Callback functions on WS frame serving
int on_data_payload(void *user_data, char *buff, size_t len);
int on_control_begin(void *user_data, enum ws_frame_type_t frame_type);
int on_control_payload(void *user_data, const char *buff, size_t len);