/**
 * @file  ws_handler.h
 * @brief handle all recived WebSocket frames 
 */

#pragma once
#include "protocols/ws_protocol/ws.h"
#include "manager_fwd.h"

/* key word to login operation  */
#define WS_LOGIN_OP "login_op"
#define WS_PASSWORD "password"

/* key word to send message operation  */
#define WS_SEND_OP "send_op"
#define WS_MESSAGE "message"
#define WS_TIME "time"

/* key word to search login operation  */
#define WS_SEARCH_OP "search_login"

/* key word to login operation  */
#define WS_SENDER_LOGIN "sender_login"

/**
 * @brief Callback function, calls on payload
 * @param user_data specific data that was passed to parser
 * @param buff pointer on parsed data
 * @param len is length of buff
 * @return error code
 */
int on_data_payload(void *user_data, char *buff, size_t len);

/**
 * @brief Callback function, calls on control frame (control type)
 * @param user_data specific data that was passed to parser
 * @param frame_type control type of recived frame
 * @return error code
 */
int on_control_begin(void *user_data, enum ws_flags frame_type);

/**
 * @brief Callback function, calls on control frame, block with payload
 * @param user_data specific data that was passed to parser
 * @param buff pointer on parsed data
 * @param len is length of buff
 * @return error code
 */
int on_control_payload(void *user_data, const char *buff, size_t len);

/**
 * @brief handle WebSocket request
 * @param ws_info connection info
 * @param frame request info
 */
void ws_handle(struct ws_routine *ws_info, struct string *frame);