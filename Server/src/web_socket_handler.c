#include "algoritms/bst.h"
#include "network_manager.h"
#include "web_socket_handler.h"
#include "parsers/json_parser.h"
#include "parsers/web_socket_builder.h"

int on_data_payload(void *user_data, char *buff, size_t len)
{
        struct web_socket_routine *ws_info = (struct web_socket_routine *)user_data;

        printf("%s\n\n", buff);
        char *request = buff;
        ++request; // Skip --> {

        json_data *dataType = parse_json_row(&request);
        printf("'%s':'%s'\n", (char *)dataType->field_name->first, (char *)dataType->field_val->first);

        /* Critical section work with shared data (shared_bst)*/
        pthread_mutex_lock(ws_info->mutex);

        if (strcmp((char *)dataType->field_name->first, WS_LOGIN) == 0)
        {
                // login
                json_data *login = parse_json_row(&request);
                json_data *password = parse_json_row(&request);
                printf("'%s':'%s'\n", (char *)login->field_name->first, (char *)login->field_val->first);       // TODO::DELETE
                printf("'%s':'%s'\n", (char *)password->field_name->first, (char *)password->field_val->first); // TODO::DELETE

                // TODO::verify login password pair
                ws_info->client_info->first = (ll)malloc(login->field_val->second * sizeof(char));
                strcpy((char *)ws_info->client_info->first, (char *)login->field_val->first);

                struct bst_node *node = bst_insert(ws_info->shared_bst, ws_info->client_info);
                printf("LOGIN = %s : FD = %d\n", node->key, node->val);

                node = bst_search(ws_info->shared_bst, (char *)login->field_val->first);
                printf("LOGIN = %s : FD = %d\n", node->key, node->val);

                postorder_traversal(ws_info->shared_bst->root);
        }
        else if (strstr((char *)dataType->field_name->first, WS_SEND_MESSAGE))
        {
                // Send message
                json_data *reciver = parse_json_row(&request);
                printf("'%s':'%s'\n", (char *)reciver->field_name->first, (char *)reciver->field_val->first); // TODO::DELETE

                struct bst_node *node = bst_search(ws_info->shared_bst, (char *)reciver->field_val->first);
                if (node)
                {
                        // Get msg
                        json_data *message = parse_json_row(&request);
                        printf("'%s':'%s'\n", (char *)message->field_name->first, (char *)message->field_val->first); // TODO::DELETE

                        printf("'reciver_socket':'%d'\n", node->val);                         // TODO::DELETE
                        printf("'sender_login':'%s'\n", (char *)ws_info->client_info->first); // TODO::DELETE
                        printf("'sender_socket':'%lld'\n", ws_info->client_info->second);     // TODO::DELETE

                        struct pair *frame = web_socket_build_frame(WS_OP_TEXT | WS_FINAL_FRAME, (char *)message->field_val->first, message->field_val->second);
                        send(node->val, (char *)frame->first, frame->second, MSG_NOSIGNAL);

                        // Write to node->key->second about new message
                        free(message);
                        free(frame);
                }
                else
                {
                        printf("Login: %s is not connected now\n", (char *)reciver->field_val->first);
                }

                free(reciver);
        }

        pthread_mutex_unlock(ws_info->mutex); /* Unlock access */

        free(dataType);
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