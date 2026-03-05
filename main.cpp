#include "Server.hpp"

int main(void)
{
    Server serv;


    for(;;) {
        int poll_count = poll(serv.pfds, serv.fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        // Run through connections looking for data to read
        serv.process_connections(serv.listener, &serv.fd_count, &serv.fd_size, &serv.pfds);
    }
}