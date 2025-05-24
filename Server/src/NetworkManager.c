#include "Dependencies.h"
#include "NetworkManager.h"
#include "Algoritms.h"
#include "FileHelper.h"
#include "ParserJSON.h"

int InitNetworkManager(struct NetworkManager *network_manager, int server_port)
{
    network_manager->addrlen = sizeof(struct sockaddr_in);
    if ((network_manager->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        return SOCKET_ERROR;
    }

    int yes = 1; // Set option to socket to ignore TIME_WAIT in tcp protocol connection
    if (setsockopt(network_manager->server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        return OPT_ERROR;
    }

    struct sockaddr_in address = network_manager->address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server_port);

    // Bind the socket to the address
    if (bind(network_manager->server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return BIND_ERROR;
    }

    // Listen for incoming connections
    if (listen(network_manager->server_fd, 3) < 0) {
        return LISTEN_ERROR;
    }
    printf("Server: START\n");

    return SUCCESS;
}

void DestroyNetworkManager(struct NetworkManager *network_manager)
{
    shutdown(network_manager->server_fd, 2); // Stop receiving&send data
    close(network_manager->server_fd);
    printf("Server: SHUTDOWN\n");
}

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

// Try to serve request
struct pair serveGetRequest(char *request)
{
    char *file_path = malloc(MAX_FILEPATH_SIZE * sizeof(char));
    
    // Form relative path to requested file
    // Append path to client files 
    strcpy(file_path, PATH_TO_CLIENT_FILES);
    int file_path_idx = strlen(PATH_TO_CLIENT_FILES);
    int read_buffer_idx = 4; // Skip "GET " to start of requested resource
    
    while (request[read_buffer_idx] != ' ') // Requested filepath shoudn't have whitespaces
    {
        file_path[file_path_idx++] = request[read_buffer_idx++];
    }
    file_path[file_path_idx] = '\0';

    //TODO::DELETE DEBUG
    if (_DEBUG)
    {
        printf("%s\n\n", request);
        printf("file path: %s\n\n", file_path);
    }
    
    struct pair response = createHTTPResponse(file_path);
    char* buffer = (char*)response.first;

    return response;
}

HTTP_POST_TYPES getPostType(char **request)
{
    HTTP_POST_TYPES type = UNKNOWN_POST;

    struct parsedData post_type = parseJSONRow(request);

    printf("\"%s\":", (char*)post_type.field_name.first);
    printf("\"%s\"\n", (char*)post_type.field_val.first);

    if (stringCompare((char*)post_type.field_name.first, HTTP_POST_LOGIN))
    {
        type = LOGIN_POST;
    } else if (stringCompare((char*)post_type.field_name.first, HTTP_POST_RECV_MESSAGES))
    {
        type = RECIVE_MESSAGES_POST;
    } else if (stringCompare((char*)post_type.field_name.first, HTTP_POST_RECV_DIALOGS))
    {
        type = RECIVE_DIALOGS_POST;
    }
    return type;
}

// Parse JSON file from client
struct pair serveLoginPost(char **request)
{
    struct pair response = {0ll, 0ll};

    // Now we have ptr to username and password
    struct parsedData username = parseJSONRow(request);
    struct parsedData password = parseJSONRow(request);
    printf("username = %s\n", (char*)username.field_val.first);
    printf("password = %s\n", (char*)password.field_val.first);

    // TODO::Select data from database

    return response;
}

struct pair serveRecvMessagesPost(char **request)
{
    struct pair response = {0ll, 0ll};

    struct parsedData sender = parseJSONRow(request);
    printf("\"%s\":", (char*)sender.field_name.first);
    printf("\"%s\"\n", (char*)sender.field_val.first);

    struct parsedData reciver = parseJSONRow(request);
    printf("\"%s\":", (char*)reciver.field_name.first);
    printf("\"%s\"\n", (char*)reciver.field_val.first);

    struct parsedData message = parseJSONRow(request);
    printf("\"%s\":", (char*)message.field_name.first);
    printf("\"%s\"\n", (char*)message.field_val.first);

    FILE *data_storage = fopen(PATH_TO_STORAGE, "w");
    if (data_storage == NULL)
    {
        return response;
    }
    fputs((char*)sender.field_val.first, data_storage);
    fputs("\n",data_storage);
    fputs((char*)reciver.field_val.first, data_storage);
    fputs("\n",data_storage);
    fputs((char*)message.field_val.first, data_storage);
    fputs("\n",data_storage);
    fclose(data_storage);

    // TODO::INPUT data to database
    // TODO::Update reciver side

    return response;
}

struct pair serveRecvDialogsPost(char **request)
{
    struct pair response = {0ll, 0ll};

    struct parsedData sender = parseJSONRow(request);
    printf("\"%s\":", (char*)sender.field_name.first);
    printf("\"%s\"\n", (char*)sender.field_val.first);

    struct parsedData reciver = parseJSONRow(request);
    printf("\"%s\":", (char*)reciver.field_name.first);
    printf("\"%s\"\n", (char*)reciver.field_val.first);

    // TODO::Select data from database

    return response;
}

// Serve post request
struct pair servePostRequest(char *request)
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
    response.first = (ll)malloc(HTTP_RESPONSE_HEADER_SIZE*sizeof(char));
    strcpy((char*)response.first, HTTP_RESPONSE_HEADER);
    response.second = HTTP_RESPONSE_HEADER_SIZE;

    return response;
}

// Accept incoming connections
void serveConnection(struct NetworkManager *network_manager)
{
    bool isServeConnection = TRUE;
    char* request = malloc(MAX_REQUEST_SIZE * sizeof(char));

    while (isServeConnection)
    {
        // Wait for request
        int client_socket = accept
        (   
            network_manager->server_fd,
            (struct sockaddr *)&network_manager->address,
            (socklen_t*)&network_manager->addrlen
        );

        // Read request
        read(client_socket,request,MAX_REQUEST_SIZE);

        // Server request
        struct pair response;
        switch(getRequestType(request))
        {
            case GET_REQUEST:
                response = serveGetRequest(request);
            break;
            case POST_REQUEST:
                response = servePostRequest(request);
            break;
            case UNKNOWN_REQUEST:
                printf("UNKNOWN REQUEST\n");
            break;
        }

        if (response.first) // When server can provide response
        {
            send(client_socket,(char*)response.first,response.second,0);
            free((char*)response.first);
        }

        // After handle, go to next client
        close(client_socket);
    }
    free(request);
}