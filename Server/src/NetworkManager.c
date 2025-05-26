#include "Dependencies.h"
#include "NetworkManager.h"
#include "Algoritms.h"
#include "FileHelper.h"
#include "WebSocketHandle.h"
#include "Server.h"

int InitNetworkManager(struct NetworkManager *network_manager, struct Server *server)
{
    network_manager->server = server;
    
    // Init BST for resolving socket-login pairs
    const int connections_start_size = 1000;
    network_manager->ws_massive = malloc(connections_start_size * sizeof(WebSocketRoutine));
    network_manager->ws_massive_max_size = connections_start_size;
    network_manager->ws_massive_cur_size = 0;

    // WebSocket parser callbacks
    wsParserCallbacks *callbacks = malloc(sizeof(wsParserCallbacks));
    network_manager->callbacks = callbacks;

    callbacks->onDataBegin      = NULL;//
    callbacks->onDataEnd        = NULL;// NOT USE
    callbacks->onControlEnd     = NULL;// 
    callbacks->onControlBegin   = onControlBegin;  // Serve PING&PONG
    callbacks->onDataPayload    = onDataPayload;   // Get data from MSGs
    callbacks->onControlPayload = onControlPayload;// Get data from PING&PONG

    if (__OUTPUT_LOGS)
    {
        printf("Server: Init struct NetworkManager\n");
    }

    return SUCCESS;
}

void DestroyNetworkManager(struct NetworkManager *network_manager)
{
    free(network_manager->callbacks);
    free(network_manager);

    if (__OUTPUT_LOGS)
    {
        printf("Server: Destroy struct NetworkManager\n");
    }
}

// void appendWSRoutine(struct struct NetworkManager *network_manager, struct WebSocketRoutine *ws_new_connection)
// {
//     if (network_manager->ws_massive_cur_size >= network_manager->ws_massive_max_size) // verify that we have memory to append new connection
//     { // expand massive by two times
//         size_t begin = network_manager->ws_massive;
//         size_t new_massive = malloc(network_manager->ws_massive_max_size<<1); // ~ *2
//         memcpy(new_massive, begin, network_manager->ws_massive_cur_size*sizeof(struct WebSocketRoutine));
        
//         network_manager->ws_massive = new_massive;
//         network_manager->ws_massive_max_size<<1; // ~ *2
//         free(begin);
//     }
//     (network_manager->ws_massive)[network_manager->ws_massive_cur_size] = ws_new_connection;
//     ++(network_manager->ws_massive_cur_size); // increase size on one new element
// }

HTTP_REQUEST getRequestType(char *request)
{
    // Verify if it is GET request
    HTTP_REQUEST response = UNKNOWN_REQUEST;
    if (stringCompare(HTTP_GET,request))
    {
        response = GET_REQUEST;
    } else if (stringCompare(HTTP_POST,request))
    {
        response = POST_REQUEST;
    }
    return response;
}

HTTP_GET_TYPES getURLType(char *request)
{
    HTTP_GET_TYPES response = STD_URL;
    if (strstr(request, HTTP_WS))
    {
        response = WEB_SOCKET_URL;
    }
    return response;
}

Pair *serverStdURL(char *request)
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
    
    Pair *response = NULL;
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

ServerResponse serveWebSocketURL(char *request)
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

    Pair *data = malloc(sizeof(Pair));
    data->first = (ll)ws_response;
    data->second = WS_HEADER_LEN+key_len+2;
    ServerResponse response = {data, TRUE};
    return response;
}

// Try to serve request
ServerResponse serveGetRequest(char *request)
{
    ServerResponse response = {NULL, FALSE};
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

HTTP_POST_TYPES getPostType(char **request)
{
    HTTP_POST_TYPES type = UNKNOWN_POST;

    JSONData *post_type = parseJSONRow(request);

    if (stringCompare((char*)post_type->field_name->first, HTTP_POST_LOGIN))
    {
        type = LOGIN_POST;
    } else if (stringCompare((char*)post_type->field_name->first, HTTP_POST_RECV_MESSAGES))
    {
        type = RECIVE_MESSAGES_POST;
    } else if (stringCompare((char*)post_type->field_name->first, HTTP_POST_RECV_DIALOGS))
    {
        type = RECIVE_DIALOGS_POST;
    }
    DestroyJSONData(post_type);
    return type;
}

// Parse JSON file from client
Pair serveLoginPost(char **request)
{
    Pair response = {0ll, 0ll};

    // Now we have ptr to username and password
    JSONData *username = parseJSONRow(request);
    JSONData *password = parseJSONRow(request);

    // TODO::Select data from database
    free(username);
    free(password);
    return response;
}

Pair serveRecvMessagesPost(char **request)
{
    Pair response = {0ll, 0ll};

    JSONData *reciver = parseJSONRow(request);
    JSONData *message = parseJSONRow(request);

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

    DestroyJSONData(reciver);
    DestroyJSONData(message);

    return response;
}

Pair serveRecvDialogsPost(char **request)
{
    Pair response = {0ll, 0ll};

    JSONData *sender = parseJSONRow(request);
    JSONData *reciver = parseJSONRow(request);

    // TODO::Select data from database

    DestroyJSONData(sender);
    DestroyJSONData(reciver);
    return response;
}

// Serve post request
Pair *servePostRequest(char *request)
{
    //TODO::SELECT INFO FROM FS OR DB TO SERVE CLIENT
    //IT CAN BE LOGIN POST OR GET CHATS INFO

    // Go to start of JSON Data
    while (*request != '{' && *request != '\0')
    {
        request++;
    }
    request++; // Skip -> {
    
    Pair response = {0ll, 0ll};
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
    Pair *response_ret = malloc(sizeof(Pair));
    response_ret->first = response.first;
    response_ret->first = response.second;
    return response_ret;
}

Pair serveWebSocket(WebSocketRoutine *ws_args, Pair frame)
{
    wsParser parser;
    ws_parser_init(&parser);
    int rc = ws_parser_execute(&parser, ws_args->network_manager->callbacks, &ws_args, (char*)frame.first, frame.second);
    if(rc != WS_OK)
    {
        printf("error: %d %s\n", rc, ws_parser_error(rc));
    }
}

void* serveConnection(void *arg)
{    
    WebSocketRoutine *ws_args = (WebSocketRoutine*)arg;
    struct NetworkManager *network_manager = (struct NetworkManager*)ws_args->network_manager;

    // Information about connection {socket, (char*)login}
    Pair *client_info = (Pair*)ws_args->client_info;
    
    Pair frame = {(ll)malloc(MAX_REQUEST_SIZE * sizeof(char)), 0ll};
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
void acceptConnection(struct NetworkManager *network_manager)
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

        // Server request
        ServerResponse response = {NULL, FALSE};
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
            WebSocketRoutine *ws_args = malloc(sizeof(WebSocketRoutine));
            // appendWSRoutine(network_manager, ws_args); // Add new connection
            pthread_t thread_id;
            
            // Fill struct fields
            ws_args->network_manager = network_manager;
            ws_args->client_info = malloc(sizeof(Pair));
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