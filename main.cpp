#include "Server.hpp"
#include "Channel.hpp"
#include <iostream>

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

/*int main()
{
    Server new_server;
    size_t i = 0;
    std::string nickname;
    std::string username;
    Client client;
    while (i < 3)
    {
        std::cout << "What's your nickname ? ";
        std::getline(std::cin, nickname);
        if (!valid_nickname(nickname))
        {
            std::cout << "You must enter a valid nickname" << std::endl;
            continue;
        }
        std::cout << "What's your username ? ";
        std::getline(std::cin, username);
        client.setNick(nickname);
        client.setUser(username);
        new_server.addClient(client);
        std::cout << std::endl;
        i++;
    }


    std::cout << new_server.clients.size() << std::endl;
    i = 0;
    while (i < new_server.clients.size())
    {
        std::cout << new_server.clients[i].getUser();
        if (i < new_server.clients.size() - 1)
            std::cout << ", ";
        i++;
    }


    std::cout << std::endl << std::endl << "Withdraw a user : ";
    std::getline(std::cin, username);
    new_server.removeClient(username);


    std::cout << new_server.clients.size() << std::endl;
    i = 0;
    while (i < new_server.clients.size())
    {
        std::cout << new_server.clients[i].getUser();
        if (i < new_server.clients.size() - 1)
            std::cout << ", ";
        i++;
    }
    return 0;
}*/