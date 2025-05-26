#include "dependencies.h"
#include "network_manager.h"
#include "algoritms.h"
#include "file_helper.h"
#include "web_socket_handler.h"
#include "server.h"

int init_network_manager(struct network_manager *network_manager, struct server *server)
{
    network_manager->server = server;
    
    // Init BST for resolving socket-login struct pairs
    const int connections_start_size = 1000;
    network_manager->ws_massive = malloc(connections_start_size * sizeof(struct web_socket_routine));
    network_manager->ws_massive_max_size = connections_start_size;
    network_manager->ws_massive_cur_size = 0;

    // WebSocket parser callbacks
    struct ws_parser_callbacks *callbacks = malloc(sizeof(struct ws_parser_callbacks));
    network_manager->callbacks = callbacks;

    callbacks->on_data_begin      = NULL;//
    callbacks->on_data_end        = NULL;// NOT USE
    callbacks->on_control_end     = NULL;// 
    callbacks->on_control_begin   = on_control_begin;  // Serve PING&PONG
    callbacks->on_data_payload    = on_data_payload;   // Get data from MSGs
    callbacks->on_control_payload = on_control_payload;// Get data from PING&PONG

    if (__OUTPUT_LOGS)
    {
        printf("server: Init struct network_manager\n");
    }

    return SUCCESS;
}

void destroy_network_manager(struct network_manager *network_manager)
{
    free(network_manager->callbacks);
    free(network_manager);

    if (__OUTPUT_LOGS)
    {
        printf("server: Destroy struct network_manager\n");
    }
}

// void appendWSRoutine(struct struct network_manager *network_manager, struct struct web_socket_routine *ws_new_connection)
// {
//     if (network_manager->ws_massive_cur_size >= network_manager->ws_massive_max_size) // verify that we have memory to append new connection
//     { // expand massive by two times
//         size_t begin = network_manager->ws_massive;
//         size_t new_massive = malloc(network_manager->ws_massive_max_size<<1); // ~ *2
//         memcpy(new_massive, begin, network_manager->ws_massive_cur_size*sizeof(struct struct web_socket_routine));
        
//         network_manager->ws_massive = new_massive;
//         network_manager->ws_massive_max_size<<1; // ~ *2
//         free(begin);
//     }
//     (network_manager->ws_massive)[network_manager->ws_massive_cur_size] = ws_new_connection;
//     ++(network_manager->ws_massive_cur_size); // increase size on one new element
// }

enum http_request getRequestType(char *request)
{
    // Verify if it is GET request
    enum http_request response = UNKNOWN_REQUEST;
    if (string_compare(HTTP_GET,request))
    {
        response = GET_REQUEST;
    } else if (string_compare(HTTP_POST,request))
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
    request += strlen(HTTP_GET)+1; // Skip "GET " to start of requested resource
    int url_size = 0;
    while (request[url_size] != ' ') // url end by whitespace
    {
        url_size++;
    }
    request[url_size] = '\0';

    int file_path_size = strlen(request) + PATH_TO_CLIENT_FILES_LEN - 1;
    char *file_path = malloc(file_path_size * sizeof(char));
    strcpy(file_path, PATH_TO_CLIENT_FILES);
    strcpy(file_path + PATH_TO_CLIENT_FILES_LEN-1, request);
    file_path[file_path_size] = '\0';
    
    //TODO::DELETE DEBUG
    if (__DEBUG)
    {
        printf("file path: %s\n", file_path);
    }
    
    struct pair *response = NULL;
    if (file_path_size-PATH_TO_CLIENT_FILES_LEN > 3) // Prevent from get files like / /. /..
    {
        response = createHTTPResponse(file_path);
        
        // TODO::Correct createHTTPResponse to return the header in any case
        if ((char*)response->first == NULL)
        {
            response->first = (ll)malloc(HTTP_RESPONSE_HEADER_SIZE * sizeof(char));
            strcpy((char*)response->first, HTTP_RESPONSE_HEADER);
            response->second = HTTP_RESPONSE_HEADER_SIZE;
        }
    }
    return response;
}

struct server_response serveWebSocketURL(char *request)
{
    // IF init connection
    char *guidkey = malloc(GUIDKEY_LEN * sizeof(char));
    char *key_field_val = strstr(request, WS_KEY_FIELD) + WS_KEY_FIELD_LEN;
    strncpy(guidkey, key_field_val, WS_KEY_LEN);
    strcpy(guidkey+WS_KEY_LEN, GUID);
    guidkey[GUIDKEY_LEN] = '\0';

    unsigned char *sha1_code = malloc(SHA_DIGEST_LENGTH * sizeof(char));
    SHA1(guidkey, GUIDKEY_LEN-1, sha1_code);

    char *key = base64simple_encode(sha1_code, strlen(sha1_code));
    int key_len = strlen(key);
    char *ws_response = malloc(WS_HEADER_LEN+key_len+2 * sizeof(char));

    strcpy(ws_response, WS_HEADER);
    strcpy(ws_response+WS_HEADER_LEN-1, key);
    ws_response[WS_HEADER_LEN+key_len-1] = '\n';
    ws_response[WS_HEADER_LEN+key_len  ] = '\n';
    ws_response[WS_HEADER_LEN+key_len+1] = '\0';
    
    free(guidkey);
    free(sha1_code);
    free(key);

    struct pair *data = malloc(sizeof(struct pair));
    data->first = (ll)ws_response;
    data->second = WS_HEADER_LEN+key_len+2;
    struct server_response response = {data, TRUE};
    return response;
}

// Try to serve request
struct server_response serveGetRequest(char *request)
{
    struct server_response response = {NULL, FALSE};
    switch (getURLType(request))
    {
        case STD_URL:
            response.data = serverStdURL(request);
        break;
        case WEB_SOCKET_URL:
            response = serveWebSocketURL(request);
        break;
    }
    return response;
}

enum http_get_types getPostType(char **request)
{
    enum http_get_types type = UNKNOWN_POST;

    json_data *post_type = parse_json_row(request);

    if (string_compare((char*)post_type->field_name->first, HTTP_POST_LOGIN))
    {
        type = LOGIN_POST;
    } else if (string_compare((char*)post_type->field_name->first, HTTP_POST_RECV_MESSAGES))
    {
        type = RECIVE_MESSAGES_POST;
    } else if (string_compare((char*)post_type->field_name->first, HTTP_POST_RECV_DIALOGS))
    {
        type = RECIVE_DIALOGS_POST;
    }
    destroy_json_data(post_type);
    return type;
}

// Parse JSON file from client
struct pair serveLoginPost(char **request)
{
    struct pair response = {0ll, 0ll};

    // Now we have ptr to username and password
    json_data *username = parse_json_row(request);
    json_data *password = parse_json_row(request);

    // TODO::Select data from database
    free(username);
    free(password);
    return response;
}

struct pair serveRecvMessagesPost(char **request)
{
    struct pair response = {0ll, 0ll};

    json_data *reciver = parse_json_row(request);
    json_data *message = parse_json_row(request);

    FILE *data_storage = fopen(PATH_TO_STORAGE, "a");
    if (data_storage == NULL)
    {
        return response;
    }
    fputs((char*)reciver->field_val->first, data_storage);
    fputs("\n",data_storage);
    fputs((char*)message->field_val->first, data_storage);
    fputs("\n",data_storage);
    fclose(data_storage);

    // TODO::INPUT data to database
    // TODO::Update reciver side

    response.first = (ll)malloc(HTTP_RESPONSE_HEADER_SIZE * sizeof(char));
    strcpy((char*)response.first, HTTP_RESPONSE_HEADER);
    response.second = HTTP_RESPONSE_HEADER_SIZE;

    destroy_json_data(reciver);
    destroy_json_data(message);

    return response;
}

struct pair serveRecvDialogsPost(char **request)
{
    struct pair response = {0ll, 0ll};

    json_data *sender = parse_json_row(request);
    json_data *reciver = parse_json_row(request);

    // TODO::Select data from database

    destroy_json_data(sender);
    destroy_json_data(reciver);
    return response;
}

// Serve post request
struct pair *servePostRequest(char *request)
{
    //TODO::SELECT INFO FROM FS OR DB TO SERVE CLIENT
    //IT CAN BE LOGIN POST OR GET CHATS INFO

    // Go to start of JSON Data
    while (*request != '{' && *request != '\0')
    {
        request++;
    }
    request++; // Skip -> {
    
    struct pair response = {0ll, 0ll};
    switch(getPostType(&request))
    {
        case UNKNOWN_POST:
        // Nothing to do
        break;
        case LOGIN_POST:
        response = serveLoginPost(&request);
        break;
        case RECIVE_MESSAGES_POST:
        response = serveRecvMessagesPost(&request);
        break;
        case RECIVE_DIALOGS_POST:
        response = serveRecvDialogsPost(&request);
        break;
    }

    // TODO::DELETE
    struct pair *response_ret = malloc(sizeof(struct pair));
    response_ret->first = response.first;
    response_ret->first = response.second;
    return response_ret;
}

struct pair serveWebSocket(struct web_socket_routine *ws_args, struct pair frame)
{
    struct ws_parser parser;
    ws_parser_init(&parser);
    int rc = ws_parser_execute(&parser, ws_args->network_manager->callbacks, &ws_args, (char*)frame.first, frame.second);
    if(rc != WS_OK)
    {
        printf("error: %d %s\n", rc, ws_parser_error(rc));
    }
}

void* serveConnection(void *arg)
{    
    struct web_socket_routine *ws_args = (struct web_socket_routine*)arg;
    struct network_manager *network_manager = (struct network_manager*)ws_args->network_manager;

    // Information about connection {socket, (char*)login}
    struct pair *client_info = (struct pair*)ws_args->client_info;
    
    struct pair frame = {(ll)malloc(MAX_REQUEST_SIZE * sizeof(char)), 0ll};
    while (TRUE)
    {   
        frame.second = recv(client_info->first, (char*)frame.first, MAX_REQUEST_SIZE, 0);
        
        if (frame.second <= 0)
        {
            printf("ID: %lld is disconnected\n", client_info->first);
            break;
        }
        ((char*)frame.first)[frame.second] = '\0';
        serveWebSocket(ws_args, frame);
    }
    close(client_info->first);
    pthread_exit(0);
}

// Accept incoming connections
void accept_connection(struct network_manager *network_manager)
{
    char *request = malloc(MAX_REQUEST_SIZE * sizeof(char));
    bool isServeConnection = TRUE;

    while (isServeConnection)
    {
        // Wait for request
        int client_socket = accept
        (   
            network_manager->server->server_fd,
            (struct sockaddr *)&network_manager->server->address,
            (socklen_t*)&network_manager->server->addrlen
        );

        // Recive data
        int read_len = recv(client_socket, request, MAX_REQUEST_SIZE, 0);
        request[read_len] = '\0';

        // server request
        struct server_response response = {NULL, FALSE};
        switch(getRequestType(request))
        {
            case GET_REQUEST:
                response = serveGetRequest(request);
            break;
            case POST_REQUEST:
                response.data = servePostRequest(request);
            break;
            case UNKNOWN_REQUEST:
                printf("UNKNOWN REQUEST\n");
            break;
        }
        
        if (response.data && response.data->first) // When server can provide response
        {
            send(client_socket, (char*)response.data->first, response.data->second, 0);
            free((char*)response.data->first);
            free((char*)response.data);
        }

        if (response.isWS) // if WebSocket then create new thread
        {
            struct web_socket_routine *ws_args = malloc(sizeof(struct web_socket_routine));
            // appendWSRoutine(network_manager, ws_args); // Add new connection
            pthread_t thread_id;
            
            // Fill struct fields
            ws_args->network_manager = network_manager;
            ws_args->client_info = malloc(sizeof(struct pair));
            ws_args->client_info->first = client_socket;
            ws_args->client_info->second = (ll)NULL;
            ws_args->pthread_id = &thread_id;
            
            pthread_create(&thread_id, NULL, serveConnection, (void*) ws_args);
            pthread_detach(thread_id);
            continue;
        }
        close(client_socket); // Else close connection
    }
    free(request);
}