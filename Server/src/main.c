#include "dependencies.h"
#include "server.h"

int main(int argc, char **argv)
{
        struct server *server = server_init(SERVER_PORT);
        server_start(server);
        server_destroy(server);
        return 0;
}