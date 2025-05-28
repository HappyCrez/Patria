#include "algoritms/bst.h"
#include "network_manager.h"
#include "web_socket_handler.h"
#include "parsers/json_parser.h"
#include "parsers/web_socket_builder.h"

static void ws_login_handle(struct web_socket_routine *ws_info, struct json_parser *json_parser)
{
        // We should to remember login, before get password, because parser will delete that data
        char *login = malloc(json_parser->field_val->second * sizeof(char));
        strcpy(login, (char *)json_parser->field_val->first);

        json_parser_execute(json_parser);                                                                     // get password
        printf("'%s':'%s'\n", (char *)json_parser->field_name->first, (char *)json_parser->field_val->first); // TODO::DELETE
        
        // TODO::verify login password pair
        char message[] = "SUCCESS"; // Now we get SUCCESS with any pswd

        // Bind socket to login and insert it to shared memory
        ws_info->client_info->first = (ll)login;
        struct bst_node *node = bst_insert(ws_info->shared_bst, ws_info->client_info);

        /**
         * Create JSON response
         * Allocate theorical size some more than need (don't reduce null-terminate char)
         * Because snprintf function require write N-1 chars
         */
        int response_len = JSON_ROW_FORMAT_LEN + WS_LOGIN_LEN + strlen(message);
        char *response = malloc(response_len * sizeof(char));

        // Redefine response_len with actual writed chars in response
        response_len = snprintf(response, response_len, JSON_ROW_FORMAT, WS_LOGIN, message);

        struct pair *frame = web_socket_build_frame(WS_OP_TEXT | WS_FINAL_FRAME, response, response_len);
        send(ws_info->client_info->second, (char *)frame->first, frame->second, MSG_NOSIGNAL);
}

static void ws_send_message_handle(struct web_socket_routine *ws_info, struct json_parser *json_parser)
{
        /* Try to find client in connection list */
        struct bst_node *node = bst_search(ws_info->shared_bst, (char *)json_parser->field_val->first);
        if (node == NULL)
        {
                printf("Login: %s is not connected now\n", (char *)json_parser->field_val->first);
                return;
        }

        json_parser_execute(json_parser);                                                                     // Get message
        printf("'%s':'%s'\n", (char *)json_parser->field_name->first, (char *)json_parser->field_val->first); // TODO::DELETE

        /* Create JSON response */
        /* Allocate theorical size some more than need (don't reduce null-terminate char)*/
        int response_len = JSON_TWO_ROW_FORMAT_LEN + WS_SEND_MESSAGE_LEN + WS_SENDER_LOGIN_LEN + ws_info->client_info->second + json_parser->field_val->second;
        char *response = malloc(response_len * sizeof(char));

        /* rewrite len on actual value */
        response_len = snprintf(response, response_len, JSON_TWO_ROW_FORMAT, WS_SEND_MESSAGE, (char *)json_parser->field_val->first, WS_SENDER_LOGIN, (char *)ws_info->client_info->first);

        struct pair *frame = web_socket_build_frame(WS_OP_TEXT | WS_FINAL_FRAME, response, response_len);
        send(node->val, (char *)frame->first, frame->second, MSG_NOSIGNAL);

        free(response);
        free(frame);
}

void ws_search_handle(struct web_socket_routine *ws_info, struct json_parser *json_parser)
{
        // TODO::Get login by input some chars
        struct bst_node *node = bst_search(ws_info->shared_bst, (char *)json_parser->field_val->first);
        if (node == NULL)
        {
                return; /* User is not on server now */
        }

        /* Create JSON response */
        /* Allocate theorical size some more than need (don't reduce null-terminate char)*/
        int response_len = JSON_ROW_FORMAT_LEN + WS_SEARCH_LEN + strlen(node->key);
        char *response = malloc(response_len * sizeof(char));

        /* rewrite len on actual value */
        response_len = snprintf(response, response_len, JSON_ROW_FORMAT, WS_SEARCH, node->key);

        struct pair *frame = web_socket_build_frame(WS_OP_TEXT | WS_FINAL_FRAME, response, response_len);
        send(ws_info->client_info->second, (char *)frame->first, frame->second, MSG_NOSIGNAL);
}

int on_data_payload(void *user_data, char *buff, size_t len)
{
        struct web_socket_routine *ws_info = (struct web_socket_routine *)user_data;

        // printf("%s\n\n", buff); //TODO::DELETE
        char *request = buff;
        ++request; // Skip --> {

        struct json_parser *json_parser = malloc(sizeof(struct json_parser));
        json_parser_init(json_parser, buff);

        if (json_parser_execute(json_parser) != SUCCESS)
        {
                return WS_INVALID_DATA;
        }
        printf("'%s':'%s'\n", (char *)json_parser->field_name->first, (char *)json_parser->field_val->first);

        /* Critical section work with shared data (shared_bst)*/
        pthread_mutex_lock(ws_info->mutex);

        if (strcmp((char *)json_parser->field_name->first, WS_LOGIN) == 0)
                ws_login_handle(ws_info, json_parser);

        else if (strstr((char *)json_parser->field_name->first, WS_SEND_MESSAGE))
                ws_send_message_handle(ws_info, json_parser);

        else if (strstr((char *)json_parser->field_name->first, WS_SEARCH))
                ws_search_handle(ws_info, json_parser);

        pthread_mutex_unlock(ws_info->mutex); /* Unlock access */
        json_parser_destroy(json_parser);

        return WS_OK;
}

int on_control_begin(void *user_data, enum websocket_flags frame_type)
{
        (void)user_data;
        printf("control_begin: %s\n",
               frame_type == WS_OP_PING ? "ping" : frame_type == WS_OP_PONG ? "pong"
                                               : frame_type == WS_OP_CLOSE  ? "close"
                                                                            : "?");
        if (frame_type == WS_OP_CLOSE)
        {
                struct web_socket_routine *ws_info = (struct web_socket_routine *)user_data;
                char reason[] = {0x03, 0xE8, 'c', 'l', 'o', 's', 'e', '\0'};
                struct pair *close_frame = web_socket_build_frame(WS_OP_CLOSE | WS_FINAL_FRAME, reason, strlen(reason));
                send(ws_info->client_info->second, (char *)close_frame->first, close_frame->second, MSG_NOSIGNAL);
        }
        return WS_OK;
}

int on_control_payload(void *user_data, const char *buff, size_t len)
{
        (void)user_data;
        printf("control_payload: '%.*s'\n", (int)len, buff);
        return WS_OK;
}