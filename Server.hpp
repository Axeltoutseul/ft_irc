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
#include <vector>
#define PORT "6667"
#include "Client.hpp"

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
        void handle_client_data(int listener, int *fd_count, struct pollfd *pfds, int *pfd_i);
        void process_connections(int listener, int *fd_count, int *fd_size, struct pollfd **pfds);

        void addClient(const Client &new_client);
        

        int listener;
        int fd_size;
        int fd_count;
        struct pollfd *pfds;
        std::vector<Client> clients;

    private :
        std::string _name;
};

int is_digit(const char *av);
int is_in_charset(int carac, const std::string charset);
int valid_nickname(const std::string nickname);

#endif