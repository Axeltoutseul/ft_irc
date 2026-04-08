#include "Server.hpp"

static bool &get_running()
{
    static bool running = true;
    return running;
}

void sig_handler(int sig)
{
    (void)sig;
    get_running() = false;
}

int main(int argc, char **argv)
{
    signal(SIGINT, sig_handler);
    if (argc < 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    int port = atoi(argv[1]);
    if (port <= 1024 || port > 65535)
    {
        std::cerr << "Error: invalid port" << std::endl;
        return 1;
    }

    Server serv(argv[1], argv[2]);

    while (get_running()) {
        int poll_count = poll(serv.pfds, serv.fd_count, -1);

        if (poll_count == -1) {
            std::cerr << "poll failed" << std::endl;
            break;
        }

        // Run through connections looking for data to read
        serv.process_connections(serv.listener, &serv.fd_count, &serv.fd_size, &serv.pfds);
    }
}