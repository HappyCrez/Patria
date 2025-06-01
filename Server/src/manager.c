#include "manager.h"
#include "server.h"

static int compare_pair_login_socket(void *data1, void *data2)
{
        struct pair *pair1 = (struct pair *)data1;
        struct pair *pair2 = (struct pair *)data2;
        return string_compare((struct string *)pair1->first, (struct string *)pair2->first);
}

struct manager *manager_init(struct server *server)
{
        struct manager *manager = malloc(sizeof(struct manager));
        manager->server = server;
        manager->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

        // Init BST for resolving login-socket struct pairs
        manager->ws_clients = bst_init(compare_pair_login_socket);

        // WebSocket parser callbacks
        struct ws_parser_callbacks *callbacks = malloc(sizeof(struct ws_parser_callbacks));
        manager->callbacks = callbacks;

        callbacks->on_data_begin = NULL;                    //
        callbacks->on_data_end = NULL;                      // not use
        callbacks->on_control_end = NULL;                   //
        callbacks->on_control_begin = on_control_begin;     // serve PING&PONG
        callbacks->on_data_payload = on_data_payload;       // get data from msgs
        callbacks->on_control_payload = on_control_payload; // get data from control frames

        if (__OUTPUT_LOGS)
                printf("manager: Created\n");
        return manager;
}

void manager_destroy(struct manager *manager)
{
        free(manager->callbacks);
        free(manager);

        if (__OUTPUT_LOGS)
                printf("manager: Destroyed \n");
}

static void *manager_handle_connection(void *arg)
{
        struct ws_routine *ws_info = (struct ws_routine *)arg;

        // Information about connection {(struct string *)login, (int64_t) socket}
        struct pair *client_info = ws_info->client_info;
        struct string *frame = string_init(MAX_REQUEST_SIZE);
        bool ws_routine_loop = true;

        while (ws_routine_loop)
        {
                frame->len = recv(client_info->second, frame->data, MAX_REQUEST_SIZE, 0);
                if (frame->len <= 0)
                {
                        printf("ID: %lld is disconnected\n", client_info->second);
                        break;
                }
                frame->data[frame->len] = '\0';
                ws_handle(ws_info, frame);
        }
        close(client_info->second);

        /**
         *  Verify that connection is not closed before login (authorization) pass
         *  and login was writed in client_info
         *  This is critical section work with shared data (shared_bst)
         */
        pthread_mutex_lock(ws_info->mutex);
        if (client_info->first != 0)
        {
                bst_delete_node(ws_info->shared_bst, client_info);
                string_destroy((struct string *)ws_info->client_info->first);
        }
        pthread_mutex_unlock(ws_info->mutex);

        free(ws_info->client_info);
        free(ws_info);
        pthread_exit(0);
}

// Accept incoming connections
void manager_handle(struct manager *manager)
{
        struct string *request = string_init(MAX_REQUEST_SIZE);
        bool handle_loop = true;

        while (handle_loop)
        {
                // Wait for request
                int client_socket = accept(
                    manager->server->server_fd,
                    (struct sockaddr *)&manager->server->address,
                    (socklen_t *)&manager->server->addrlen);

                // Recive data
                request->len = recv(client_socket, request->data, MAX_REQUEST_SIZE, 0);
                request->data[request->len] = '\0';

                // server request
                struct http_response *http = null;

                switch (http_request_type(request))
                {
                case GET_REQUEST:
                        http = http_handle_get(request);
                        break;

                /* server don't handle post requests */
                case POST_REQUEST:
                        http = malloc(sizeof(struct http_response));
                        http->is_ws = false;
                        http->response = http_404();
                        break;

                /* usally error in socket transmiting data */
                case UNKNOWN_REQUEST:
                        if (__OUTPUT_LOGS)
                                printf("Unknown request!\n");
                        http = malloc(sizeof(struct http_response));
                        http->is_ws = false;
                        http->response = http_404();
                        break;
                }

                if (http->response) // When server can provide response
                {
                        send(client_socket,
                             http->response->data,
                             http->response->len,
                             MSG_NOSIGNAL);

                        //printf("Response:\n%s\nLenght = %d\n", http->response->data, http->response->len);
                }

                /* on std http request -> close connection */
                if (!http->is_ws)
                {
                        close(client_socket);
                        string_destroy(http->response);
                        free(http);
                        continue;
                }
                string_destroy(http->response);
                free(http);

                /* on http upgrade to WebSocket -> create communication thread */
                struct ws_routine *ws_args = malloc(sizeof(struct ws_routine));
                pthread_t *thread_id = malloc(sizeof(pthread_t));
                
                ws_args->pthread = thread_id;
                ws_args->mutex = &manager->mutex;
                ws_args->callbacks = manager->callbacks;
                ws_args->shared_bst = manager->ws_clients;
                ws_args->client_info = malloc(sizeof(struct pair));
                ws_args->client_info->first = 0;
                ws_args->client_info->second = client_socket;

                if (__OUTPUT_LOGS)
                        printf("ID: %d is connected\n", client_socket);

                pthread_create(thread_id, null, manager_handle_connection, (void *)ws_args);
                pthread_detach(*thread_id);
        }
        string_destroy(request);
}