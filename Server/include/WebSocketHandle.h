#pragma once
#include "Dependencies.h"

// Callback functions on WS frame serving
int onDataPayload(void* user_data, const char* buff, size_t len);
int onControlBegin(void* user_data, ws_frame_type_t frame_type);
int onControlPayload(void* user_data, const char* buff, size_t len);