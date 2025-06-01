#include "protocols/ws_protocol/ws_handler.h"
#include "manager.h"

void ws_handle(struct ws_routine *ws_info, struct string *frame)
{
        printf("%s\n", "ws_handle_in");
        struct ws_parser *parser = ws_parser_init(ws_info->callbacks);
        int rc = ws_parser_execute(parser, (void *)ws_info, frame->data, frame->len);
        if (rc != WS_OK)
                printf("web_socket parser failed: %d %s\n", rc, ws_parser_error(rc));
        free(parser);
}

static void ws_login_handle(struct ws_routine *ws_info, struct json_parser *json_parser)
{
        // We should to remember login, before get password, because parser will delete that data
        struct string *login = string_init(json_parser->field_val->len);
        strcpy(login->data, json_parser->field_val->data);

        json_parser_execute(json_parser); // get password
        printf("'%s':'%s'\n", json_parser->field_name->data, json_parser->field_val->data);

        // TODO::verify login password pair

        char message[] = "SUCCESS"; // Now we get SUCCESS with any pswd

        // Bind socket to login and insert it to shared memory
        ws_info->client_info->first = (int64_t)login;
        struct bst_node *node = bst_insert(ws_info->shared_bst, ws_info->client_info);

        /**
         * Create JSON response
         * Allocate theorical size some more than need (don't reduce null-terminate char)
         * Because snprintf function require write N-1 chars
         */
        struct string *response = string_init(sizeof(JSON_ROW_FORMAT) + sizeof(WS_LOGIN_OP) + strlen(message));

        // Redefine response_len with actual writed chars in response
        response->len = snprintf(response->data, response->len, JSON_ROW_FORMAT, WS_LOGIN_OP, message);

        struct string *frame = ws_build_frame(WS_OP_TEXT | WS_FINAL_FRAME, response->data, response->len);
        send(ws_info->client_info->second, frame->data, frame->len, MSG_NOSIGNAL);
}

static void ws_send_message_handle(struct ws_routine *ws_info, struct json_parser *json_parser)
{
        /* Try to find client in connection list */
        struct pair *login_search = malloc(sizeof(struct pair));
        login_search->first = (int64_t)json_parser->field_val;

        struct bst_node *node = bst_search(ws_info->shared_bst, login_search);
        free(login_search);

        if (node == null)
        {
                printf("Login: %s is not connected now\n", json_parser->field_val->data);
                return;
        }

        json_parser_execute(json_parser); /* Get message */
        printf("'%s':'%s'\n", json_parser->field_name->data, json_parser->field_val->data);

        /* Create JSON response */
        /* Allocate theorical size some more than need (don't reduce null-terminate char)*/
        struct string *response = string_init(sizeof(JSON_TWO_ROW_FORMAT) +
                                              sizeof(WS_SEND_OP) +
                                              sizeof(WS_MESSAGE) +
                                              ((struct string *)ws_info->client_info->first)->len +
                                              json_parser->field_val->len);

        /* rewrite len on actual value */
        response->len = snprintf(response->data,
                                 response->len,
                                 JSON_TWO_ROW_FORMAT,
                                 WS_SEND_OP,
                                 ((struct string *)ws_info->client_info->first)->data,
                                 WS_MESSAGE,
                                 json_parser->field_val->data);

        struct string *frame = ws_build_frame(WS_OP_TEXT | WS_FINAL_FRAME, response->data, response->len);
        struct pair *client = (struct pair *)node->data;
        send(((struct pair *)node->data)->second, frame->data, frame->len, MSG_NOSIGNAL);

        string_destroy(response);
        string_destroy(frame);
}

void ws_search_handle(struct ws_routine *ws_info, struct json_parser *json_parser)
{
        /* @todo get login by input first part of chars */
        struct pair *login_search = malloc(sizeof(struct pair));
        login_search->first = (int64_t)json_parser->field_val;

        struct bst_node *node = bst_search(ws_info->shared_bst, login_search);

        free(login_search);

        if (node == null)
        {
                return; /* User is not on server now */
        }

        /* Create JSON response */
        /* Allocate theorical size some more than need (don't reduce null-terminate char)*/
        struct string *search_value = (struct string *)((struct pair *)node->data)->first;
        struct string *response = string_init(sizeof(JSON_ROW_FORMAT) +
                                              sizeof(WS_SEARCH_OP) +
                                              search_value->len);

        /* rewrite len on actual value */
        response->len = snprintf(response->data,
                                 response->len,
                                 JSON_ROW_FORMAT,
                                 WS_SEARCH_OP,
                                 search_value->data);

        struct string *frame = ws_build_frame(WS_OP_TEXT | WS_FINAL_FRAME,
                                              response->data,
                                              response->len);
        send(ws_info->client_info->second, frame->data, frame->len, MSG_NOSIGNAL);
}

int on_data_payload(void *user_data, char *buff, size_t len)
{
        struct ws_routine *ws_info = (struct ws_routine *)user_data;

        printf("%s\n\n", buff);
        struct string *request = string_init(len);
        strncpy(request->data, buff, len);

        struct json_parser *json_parser = json_parser_init(request);

        if (json_parser_execute(json_parser) != true)
                return WS_INVALID_DATA;
        
        printf("'%s':'%s'\n", json_parser->field_name->data, json_parser->field_val->data);

        /* Critical section work with shared data (shared_bst) */
        pthread_mutex_lock(ws_info->mutex);
        if (strstr(json_parser->field_name->data, WS_LOGIN_OP))
                ws_login_handle(ws_info, json_parser);

        else if (strstr(json_parser->field_name->data, WS_SEND_OP))
                ws_send_message_handle(ws_info, json_parser);

        else if (strstr(json_parser->field_name->data, WS_SEARCH_OP))
                ws_search_handle(ws_info, json_parser);

        pthread_mutex_unlock(ws_info->mutex); /* Unlock access */
        json_parser_destroy(json_parser);

        return WS_OK;
}

int on_control_begin(void *user_data, enum ws_flags frame_type)
{
        printf("control_begin: %s\n",
               frame_type == WS_OP_PING ? "ping" : frame_type == WS_OP_PONG ? "pong"
                                               : frame_type == WS_OP_CLOSE  ? "close"
                                                                            : "?");
        if (frame_type == WS_OP_CLOSE)
        {
                struct ws_routine *ws_info = (struct ws_routine *)user_data;
                char reason[] = {0x03, 0xE8, 'c', 'l', 'o', 's', 'e', '\0'};
                struct string *close_frame = ws_build_frame(WS_OP_CLOSE | WS_FINAL_FRAME, reason, strlen(reason));
                send(ws_info->client_info->second, close_frame->data, close_frame->len, MSG_NOSIGNAL);
        }
        return WS_OK;
}

int on_control_payload(void *user_data, const char *buff, size_t len)
{
        printf("control_payload: '%.*s'\n", (int)len, buff);
        return WS_OK;
}