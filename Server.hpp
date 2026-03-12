#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <iostream>
#include <vector>
#define PORT "6667"
#include "Client.hpp"
#include "utils.hpp"


typedef struct Message
{
    char *prefix;
    char *command;
    char **params;
}Mesage;

class Server
{
    public :
        Server();
        ~Server();

        const char *inet_ntop2(void *addr, char *buf, size_t size);
        int         get_listener_socket(void);

        void add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count, int *fd_size);
        void del_from_pfds(struct pollfd pfds[], int i, int *fd_count);

        void handle_new_connection(int listener, int *fd_count, int *fd_size, struct pollfd **pfds);
        void get_new_client_data();
        void handle_client_data(int listener, int *fd_count, struct pollfd *pfds, int *pfd_i);
        void process_connections(int listener, int *fd_count, int *fd_size, struct pollfd **pfds);

        void addClient(const Client &new_client);
        void removeClient(const std::string nickname);


        int listener;
        int fd_size;
        int fd_count;
        struct pollfd *pfds;
        std::vector<Client> clients;

    private :
        std::string _name;
};

Message *parseMessage(char *in_msg);

bool is_digit(const char *av);
bool is_in_charset(int carac, const std::string charset);
bool nickname_in_list(const std::string nickname, std::vector<Client> clients);
bool valid_nickname(const std::string nickname);

#endif