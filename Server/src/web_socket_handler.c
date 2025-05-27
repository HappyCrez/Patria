#include "algoritms/bst.h"
#include "network_manager.h"
#include "web_socket_handler.h"
#include "parsers/json_parser.h"
#include "parsers/web_socket_builder.h"

static void ws_login_handle(struct web_socket_routine *ws_info, struct pair *login, char *request)
{
        // login
        json_data *password = parse_json_row(&request);                                             // TODO::DELETE
        printf("'%s':'%s'\n", (char *)password->field_name->first, (char *)password->field_val->first); // TODO::DELETE

        // TODO::verify login password pair

        ws_info->client_info->first = (ll)malloc(login->second * sizeof(char));
        strcpy((char *)ws_info->client_info->first, (char *)login->first);

        struct bst_node *node = bst_insert(ws_info->shared_bst, ws_info->client_info);

        char message[] = "SUCCESS";

        /* Create JSON response */
        /* Allocate theorical size some more than need (don't reduce null-terminate char)*/
        int response_len = JSON_ROW_FORMAT_LEN + WS_LOGIN_LEN + strlen(message);
        char *response = malloc(response_len * sizeof(char));

        /* rewrite len on actual value */
        response_len = snprintf(response, response_len, JSON_ROW_FORMAT, WS_LOGIN, message);

        struct pair *frame = web_socket_build_frame(WS_OP_TEXT | WS_FINAL_FRAME, response, response_len);
        send(ws_info->client_info->second, (char *)frame->first, frame->second, MSG_NOSIGNAL);
}

static void ws_send_message_handle(struct web_socket_routine *ws_info, char *reciver, char *request)
{
        /* Try to find client in connection list */
        struct bst_node *node = bst_search(ws_info->shared_bst, reciver);
        if (node == NULL)
        {
                printf("Login: %s is not connected now\n", reciver);
                return;
        }

        // Get msg
        json_data *message = parse_json_row(&request);
        
        /* Create JSON response */
        /* Allocate theorical size some more than need (don't reduce null-terminate char)*/
        int response_len = JSON_ROW_FORMAT_LEN + WS_SEND_MESSAGE_LEN + message->field_val->second;
        char *response = malloc(response_len * sizeof(char));

        /* rewrite len on actual value */
        response_len = snprintf(response, response_len, JSON_ROW_FORMAT, WS_SEND_MESSAGE, (char *)message->field_val->first);

        struct pair *frame = web_socket_build_frame(WS_OP_TEXT | WS_FINAL_FRAME, response, response_len);
        send(node->val, (char *)frame->first, frame->second, MSG_NOSIGNAL);

        free(response);
        free(message);
        free(frame);
}

void ws_search_handle(struct web_socket_routine *ws_info, char *login)
{
        // TODO::Aproximate login by input some chars
        struct bst_node *node = bst_search(ws_info->shared_bst, login);
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

        // printf("%s\n\n", buff);
        char *request = buff;
        ++request; // Skip --> {

        json_data *data_type = parse_json_row(&request);
        printf("'%s':'%s'\n", (char *)data_type->field_name->first, (char *)data_type->field_val->first);

        /* Critical section work with shared data (shared_bst)*/
        pthread_mutex_lock(ws_info->mutex);

        if (strcmp((char *)data_type->field_name->first, WS_LOGIN) == 0)
                ws_login_handle(ws_info, data_type->field_val, request);

        else if (strstr((char *)data_type->field_name->first, WS_SEND_MESSAGE))
                ws_send_message_handle(ws_info, (char *)data_type->field_val->first, request);

        else if (strstr((char *)data_type->field_name->first, WS_SEARCH))
                ws_search_handle(ws_info, (char *)data_type->field_val->first);

        pthread_mutex_unlock(ws_info->mutex); /* Unlock access */
        
        /**
         * Not destroy json_data, because it has ptr's to request data 
         * request data will be free in upper level func
         * on this level (callback) we only use & modify, buff data, but don't delete
         */
        free(data_type);
        
        return WS_OK;
}

int on_control_begin(void *user_data, enum ws_frame_type_t frame_type)
{
        (void)user_data;
        printf("control_begin: %s\n",
               frame_type == WS_FRAME_PING ? "ping" : frame_type == WS_FRAME_PONG ? "pong"
                                                  : frame_type == WS_FRAME_CLOSE  ? "close"
                                                                                  : "?");
        if (frame_type != WS_FRAME_CLOSE)
        {
                return WS_OK;
        }
        struct web_socket_routine *ws_info = (struct web_socket_routine *)user_data;
        char reason[] = "Correct close";
        struct pair *close_frame = web_socket_build_frame(WS_OP_CLOSE | WS_FINAL_FRAME, reason, strlen(reason));
        send(ws_info->client_info->second, (char *)close_frame->first, close_frame->second, MSG_NOSIGNAL);
        return WS_OK;
}

int on_control_payload(void *user_data, const char *buff, size_t len)
{
        (void)user_data;
        printf("control_payload: '%.*s'\n", (int)len, buff);
        return WS_OK;
}