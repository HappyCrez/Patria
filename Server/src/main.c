#include "dependencies.h"
#include "server.h"

void showErrorCode(int error_code)
{
        switch (error_code)
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

int main(int argc, char **argv)
{
        struct server *server;
        int code = server_init(server, SERVER_PORT);
        if (code != SUCCESS)
        {
                showErrorCode(code);
                exit(code);
        }

        /*
         * Infinity loop
         * Can be breaken by server fault or
         * TODO::Special admin post request
         */

        server_start(server);
        server_destroy(server);
        return 0;

        /*
         * char buffer = {0};
         * SSL_CTX *ctx;
         *
         * Initialize SSL context
         * ctx = init_context();
         *
         * Create SSL structure
         * SSL *ssl = SSL_new(ctx);
         * SSL_set_fd(ssl, new_socket);
         *
         * Accept SSL connection
         * if (SSL_accept(ssl) <= 0) {
         *     perror("SSL accept failed");
         *     ERR_print_errors_fp(stderr);
         * } else {
         *     // Read and write data
         *     SSL_read(ssl, buffer, 1024);
         *     SSL_write(ssl, "HTTP/1.1 200 OK\n\nHello, World!", strlen("HTTP/1.1 200 OK\n\nHello, World!"));
         * }
         *
         * Clean up
         * SSL_free(ssl);
         * SSL_CTX_free(ctx);
         */
}