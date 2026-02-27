#include "ft_irc.hpp"

void create_server(int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    if (server_fd != 0)
    {
        std::cerr << "socket failed" << std::endl;
        return;
    }
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;

    int result = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (result < 0)
    {
        std::cerr << "bind failed" << std::endl;
        return;
    }
    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "listen failed" << std::endl;
        return;
    }
}