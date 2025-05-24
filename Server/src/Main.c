#include "Dependencies.h"
#include "NetworkManager.h"

void showErrorCode(int error_code)
{
    switch(error_code)
    {
        case SOCKET_ERROR:
            perror("Socket failed");
        break;
        case OPT_ERROR:
            perror("Configuring socket failed");
        break;
        case BIND_ERROR:
            perror("Bind failed");
        break;
        case LISTEN_ERROR:
            perror("Listen failed");
        break;
    }
}

int main(int argc, char** argv)
{
    struct NetworkManager network_manager;
    int code = InitNetworkManager(&network_manager, SERVER_PORT);
    if (code != SUCCESS)
    {
        showErrorCode(code);
        exit(code);
    }
    
    // Infinity loop
    // Can be breaken by server fault or 
    // TODO::Special admin post request
    serveConnection(&network_manager);

    DestroyNetworkManager(&network_manager);
    return 0;

    //char buffer = {0};
    // SSL_CTX *ctx;

    // Initialize SSL context
    // ctx = init_context();


    // int READ_BUFFER_SIZE = 1000;
    // char* read_buffer = malloc(READ_BUFFER_SIZE);
    // int MAX_FILEPATH_SIZE = 100;
    // char* file_path = malloc(MAX_FILEPATH_SIZE);
    
    // while (TRUE)
    // {
    //     // Accept incoming connections
    //     if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
    //     {
    //         perror("Accept failed");
    //         exit(EXIT_FAILURE);
    //     }

    //     read(new_socket,read_buffer,READ_BUFFER_SIZE);

    //     // Verify if it is GET request
    //     bool is_get_request = TRUE;
    //     for (int i = 0; HTTP_GET[i] != '\0'; i++)
    //     {
    //         if (HTTP_GET[i] != read_buffer[i])
    //         {
    //             is_get_request = FALSE;
    //             break;
    //         }
    //     }
        
    //     if (is_get_request)
    //     {
    //         strcpy(file_path,FILEPATH_START);
    //         int file_path_idx = strlen(FILEPATH_START), read_buffer_idx = 4;
    //         while (read_buffer[read_buffer_idx] != ' ')
    //         {
    //             file_path[file_path_idx++] = read_buffer[read_buffer_idx++];
    //         }
    //         file_path[file_path_idx] = '\0';
    //         printf("%s\n\n", file_path);
    //         char* buffer = readFile(file_path);
    //         if (buffer != NULL)
    //         {
    //             printf("\n%s\n", buffer);
    //             send(new_socket,buffer,strlen(buffer),0);
    //         }
    //         free(buffer);
    //         close(new_socket);
    //         continue;
    //     }

    //     bool is_post_request = TRUE;
    //     if (is_post_request)
    //     {
    //         printf("%s\n\n", read_buffer);
    //         break;
    //     }
    // }

    // Create SSL structure
    // SSL *ssl = SSL_new(ctx);
    // SSL_set_fd(ssl, new_socket);

    // Accept SSL connection
    // if (SSL_accept(ssl) <= 0) {
    //     perror("SSL accept failed");
    //     ERR_print_errors_fp(stderr);
    // } else {
    //     // Read and write data
    //     SSL_read(ssl, buffer, 1024);
    //     SSL_write(ssl, "HTTP/1.1 200 OK\n\nHello, World!", strlen("HTTP/1.1 200 OK\n\nHello, World!"));
    // }

    // // Clean up
    // SSL_free(ssl);
    // SSL_CTX_free(ctx);
    
}