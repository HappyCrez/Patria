#include "dependencies.h"
#include "algoritms/common_base_algoritms.h"
#include "algoritms/base64_coder.h"
#include "algoritms/bst.h"

#include "web_socket_handler.h"
#include "network_manager.h"
#include "file_helper.h"
#include "server.h"

#include "parsers/web_socket_builder.h"

int network_manager_init(struct network_manager *network_manager, struct server *server)
{
        network_manager->server = server;
        network_manager->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

        // Init BST for resolving login-socket struct pairs
        network_manager->ws_clients = malloc(sizeof(struct bst));
        bst_init(network_manager->ws_clients);

        // WebSocket parser callbacks
        struct ws_parser_callbacks *callbacks = malloc(sizeof(struct ws_parser_callbacks));
        network_manager->callbacks = callbacks;

        callbacks->on_data_begin = NULL;                    //
        callbacks->on_data_end = NULL;                      // NOT USE
        callbacks->on_control_end = NULL;                   //
        callbacks->on_control_begin = on_control_begin;     // Serve PING&PONG
        callbacks->on_data_payload = on_data_payload;       // Get data from MSGs
        callbacks->on_control_payload = on_control_payload; // Get data from PING&PONG

        if (__OUTPUT_LOGS)
        {
                printf("Network_manager: Created\n");
        }

        return SUCCESS;
}

void network_manager_destroy(struct network_manager *network_manager)
{
        free(network_manager->callbacks);
        free(network_manager);

        if (__OUTPUT_LOGS)
        {
                printf("Network_manager: Destroyed \n");
        }
}

enum http_request getRequestType(char *request)
{
        // Verify if it is GET request
        enum http_request response = UNKNOWN_REQUEST;
        if (strstr(request, HTTP_GET))
        {
                response = GET_REQUEST;
        }
        else if (strstr(request, HTTP_POST))
        {
                response = POST_REQUEST;
        }
        return response;
}

enum http_get_types getURLType(char *request)
{
        enum http_get_types response = STD_URL;
        if (strstr(request, HTTP_WS))
        {
                response = WEB_SOCKET_URL;
        }
        return response;
}

struct pair *serverStdURL(char *request)
{
        request += strlen(HTTP_GET) + 1; // Skip "GET " to start of requested resource
        int url_size = 0;
        while (request[url_size] != ' ') // url end by whitespace
        {
                url_size++;
        }
        request[url_size] = '\0';

        char *file_path;
        if (request[url_size - 1] == '/')
        { /* url to catalog "/" */
                file_path = malloc(PATH_TO_SRC_FILE_LEN);
                strcpy(file_path, PATH_TO_SRC_FILE);
        }
        else
        {
                file_path = malloc(strlen(request) + PATH_TO_CLIENT_FILES_LEN - 1 * sizeof(char));
                strcpy(file_path, PATH_TO_CLIENT_FILES);
                strcpy(file_path + PATH_TO_CLIENT_FILES_LEN - 1, request);
        }
        return create_http_response(file_path);
}

// IF init connection
struct pair *serveWebSocketURL(char *request)
{
        struct pair *response = NULL;
        char *guidkey = malloc(GUIDKEY_LEN * sizeof(char));
        memset(guidkey, 0, GUID_LEN);
        char *key_field_val = strstr(request, WS_KEY_FIELD) + WS_KEY_FIELD_LEN-1;
        if (key_field_val == NULL)
                return response;

        strncpy(guidkey, key_field_val, WS_KEY_LEN);
        guidkey[WS_KEY_LEN] = '\0';
        strcat(guidkey, GUID);

        unsigned char *sha1_code = malloc(SHA_DIGEST_LENGTH * sizeof(char));
        SHA1(guidkey, GUIDKEY_LEN - 1, sha1_code);

        char *key = base64simple_encode(sha1_code, strlen(sha1_code));
        int key_len = strlen(key);

        #define END_SEQUENCE_LEN 3
        #define END_SEQUENCE "\n\n"
        
        response = malloc(sizeof(struct pair));
        response->second = WS_HEADER_LEN + key_len + END_SEQUENCE_LEN - 2;
        response->first = (ll)malloc(response->second * sizeof(char));

        strcpy((char *)response->first, WS_HEADER);
        strcpy((char *)response->first + WS_HEADER_LEN - 1, key);
        strcpy((char *)response->first + WS_HEADER_LEN + key_len - 1, END_SEQUENCE);
        ((char *)response->first)[WS_HEADER_LEN + key_len + END_SEQUENCE_LEN - 2] = '\0';

        free(guidkey);
        free(sha1_code);
        free(key);

        return response;
}

// Try to serve request
struct server_response *serveGetRequest(char *request)
{
        struct server_response *response = malloc(sizeof(struct server_response));
        memset(response, 0, sizeof(response));

        switch (getURLType(request))
        {
        case STD_URL:
                response->isWS = FALSE;
                response->data = serverStdURL(request);
                break;
        case WEB_SOCKET_URL:
                response->isWS = TRUE;
                response->data = serveWebSocketURL(request);
                break;
        }
        return response;
}

enum http_get_types getPostType(char **request)
{
        enum http_get_types type = UNKNOWN_POST;

        json_data *post_type = parse_json_row(request);

        if (strstr((char *)post_type->field_name->first, HTTP_POST_LOGIN))
        {
                type = LOGIN_POST;
        }
        else if (strstr((char *)post_type->field_name->first, HTTP_POST_RECV_MESSAGES))
        {
                type = RECIVE_MESSAGES_POST;
        }
        else if (strstr((char *)post_type->field_name->first, HTTP_POST_RECV_DIALOGS))
        {
                type = RECIVE_DIALOGS_POST;
        }
        json_data_destroy(post_type);
        return type;
}

struct pair serveWebSocket(struct web_socket_routine *ws_info, struct pair frame)
{
        struct ws_parser parser;
        ws_parser_init(&parser);
        int rc = ws_parser_execute(&parser, ws_info->callbacks, (void *)ws_info, (char *)frame.first, frame.second);
        if (rc != WS_OK)
        {
                printf("web_socket parser failed: %d %s\n", rc, ws_parser_error(rc));
        }
}

void *serveConnection(void *arg)
{
        struct web_socket_routine *ws_info = (struct web_socket_routine *)arg;
        
        // Information about connection {(char*)login, socket}
        struct pair *client_info = (struct pair *)ws_info->client_info; 
        struct pair frame = {(ll)malloc(MAX_REQUEST_SIZE * sizeof(char)), 0ll};
        while (TRUE)
        {
                frame.second = recv(client_info->second, (char *)frame.first, MAX_REQUEST_SIZE, 0);
                if (frame.second <= 0)
                {
                        printf("ID: %lld is disconnected\n", client_info->second);
                        break;
                }
                ((char *)frame.first)[frame.second] = '\0';
                serveWebSocket(ws_info, frame);
        }
        close(client_info->second);

        /**
         *  Verify that connection is not closed before login write in client_info
         *  And exist node with that login
         *  This is critical section work with shared data (shared_bst)
         */
        pthread_mutex_lock(ws_info->mutex);
        if (client_info->first && bst_search(ws_info->shared_bst, (char *)client_info->first))
                bst_delete_node(ws_info->shared_bst, (char *)client_info->first);
        pthread_mutex_unlock(ws_info->mutex);

        free(ws_info);
        pthread_exit(0);
}

// Accept incoming connections
void network_manager_accept_connection(struct network_manager *network_manager)
{
        char *request = malloc(MAX_REQUEST_SIZE * sizeof(char));
        bool isServeConnection = TRUE;

        while (isServeConnection)
        {
                // Wait for request
                int client_socket = accept(
                    network_manager->server->server_fd,
                    (struct sockaddr *)&network_manager->server->address,
                    (socklen_t *)&network_manager->server->addrlen);

                // Recive data
                int read_len = recv(client_socket, request, MAX_REQUEST_SIZE, 0);
                request[read_len] = '\0';

                // server request
                struct server_response *response = NULL;

                switch (getRequestType(request))
                {
                case GET_REQUEST:
                        response = serveGetRequest(request);
                        break;
                case POST_REQUEST: // SERVER DON'T ANSWER ON POST REQUESTS
                        response = malloc(sizeof(response));
                        response->isWS = FALSE;
                        response->data = response_http_404();
                        break;
                case UNKNOWN_REQUEST:
                        printf("UNKNOWN REQUEST\n");
                        printf("\n=================\n%s\n===================\n", request);
                        response = malloc(sizeof(response));
                        response->isWS = FALSE;
                        response->data = response_http_404();
                        break;
                }

                if (response->data && response->data->first) // When server can provide response
                {
                        send(client_socket, (char *)response->data->first, response->data->second, 0);
                        free((char *)response->data->first);
                        free((char *)response->data);
                }

                /* On Http request after answer -> close connection */
                if (!response->isWS)
                {
                        close(client_socket);
                        continue;
                }

                /* On WebSocket -> create new thread */
                struct web_socket_routine *ws_args = malloc(sizeof(struct web_socket_routine));
                pthread_t *thread_id = malloc(sizeof(pthread_t));

                // Fill struct fields
                ws_args->mutex = &network_manager->mutex;
                ws_args->callbacks = network_manager->callbacks;
                ws_args->shared_bst = network_manager->ws_clients;
                ws_args->client_info = malloc(sizeof(struct pair));
                ws_args->client_info->first = (ll)NULL;
                ws_args->client_info->second = client_socket;
                ws_args->pthread = thread_id;

                printf("ID: %d is connected\n", client_socket);

                pthread_create(thread_id, NULL, serveConnection, (void *)ws_args);
                pthread_detach(*thread_id);
        }
        free(request);
}