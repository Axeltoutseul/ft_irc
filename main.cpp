#include "Server.hpp"
#include "Channel.hpp"
#include <iostream>

/*int main(void)
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
}*/

int main()
{
    Server new_server;
    Client client;
    std::string nickname;
    std::string username;
    std::cout << "What's your nickname ? ";
    std::getline(std::cin, nickname);
    std::cout << "What's your username ? ";
    std::getline(std::cin, username);
    client.setNick(nickname);
    client.setUser(username);
    new_server.addClient(client);
    std::cout << new_server.clients.size() << std::endl;
    std::cout << "Client : " << new_server.clients[0]->getNick() << ", " << new_server.clients[0]->getUser() << "." << std::endl;
    return 0;
}