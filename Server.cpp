#include "Server.hpp"

Server::Server() {
    fd_size = 5;
    fd_count = 0;
    //pfds = (pollfd *)malloc(sizeof *pfds * fd_size);
    pfds = new pollfd[fd_size];

    // Set up and get a listening socket
    listener = get_listener_socket();

    if (listener == -1) {
        std::cerr << "error getting listening socket" << std::endl;
        exit(1);
    }

    // Add the listener to set;
    // Report ready to read on incoming connection
    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1; // For the listener
    std::cout << "pollserver: waiting for connections..." << std::endl;
}

Server::~Server() {
    //free(pfds);
    delete[] pfds;
}

const char *Server::inet_ntop2(void *addr, char *buf, size_t size)
{
    struct sockaddr_storage *sas = (sockaddr_storage *)addr;
    struct sockaddr_in *sa4;
    struct sockaddr_in6 *sa6;
    void *src;
    switch (sas->ss_family) {
        case AF_INET:
            sa4 = (sockaddr_in *)addr;
            src = &(sa4->sin_addr);
            break;
        case AF_INET6:
            sa6 = (sockaddr_in6 *)addr;
            src = &(sa6->sin6_addr);
            break;
        default:
            return NULL;
    }
    return inet_ntop(sas->ss_family, src, buf, size);
}
/*
tps://beej.us/guide/bgnet/source/examples/pollserver.c
pter 7. Slightly Advanced Techniques 45
 * Return a listening socket.
 */
int Server::get_listener_socket(void)
{
    int listener; // Listening socket descriptor
    int yes=1; // For setsockopt() SO_REUSEADDR, below
    int rv;
    struct addrinfo hints, *ai, *p;
    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        std::cerr << "pollserver: " << gai_strerror(rv) << std::endl;
        exit(1);
    }
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol);
        if (listener < 0) {
            continue;
        }
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }
    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }
    freeaddrinfo(ai); // All done with this
                      // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }
    return listener;
}
/*
 * Add a new file descriptor to the set.
 */
void Server::add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count,
        int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it
        *pfds = (pollfd *)realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read
    (*pfds)[*fd_count].revents = 0;

    (*fd_count)++;
}

/*
 * Remove a file descriptor at a given index from the set.
 */
void Server::del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one  
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

/*
 * Handle incoming connections.
 */

void Server::get_new_client_data()
{
    std::string nickname;
    std::string username;
    Client client;

    std::cout << "What's your nickname ? ";
    std::getline(std::cin, nickname);
    while (!valid_nickname(nickname))
        std::cout << "You must enter a valid nickname" << std::endl;
    while (nickname_in_list(nickname, clients))
    {
        std::cout << "The nickname " << nickname << " is already taken." << std::endl;
        std::cout << "What's your nickname ? ";
        std::getline(std::cin, nickname);
    }
    std::cout << "What's your username ? ";
    std::getline(std::cin, username);
    client.setNick(nickname);
    client.setUser(username);
    client.setStateRegister(true); 
    addClient(client);
}

void Server::handle_new_connection(int listener, int *fd_count,
        int *fd_size, struct pollfd **pfds)
{
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    int newfd; // Newly accept()ed socket descriptor
    char remoteIP[INET6_ADDRSTRLEN];

    addrlen = sizeof(remoteaddr);
    newfd = accept(listener, (struct sockaddr *)&remoteaddr,
            &addrlen);

    if (newfd == -1) {
        perror("accept");
    } else {
        add_to_pfds(pfds, newfd, fd_count, fd_size);

        printf("pollserver: new connection from %s on socket %d\n",
                inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP),
                newfd);
    }
    get_new_client_data();
    std::cout << "Size = " << clients.size() << std::endl;
}

/*
 * Handle regular client data or client hangups.
 */
void Server::handle_client_data(int listener, int *fd_count,
        struct pollfd *pfds, int *pfd_i)
{
    char buf[256]; // Buffer for client data

    int nbytes = recv(pfds[*pfd_i].fd, buf, sizeof buf, 0);

    int sender_fd = pfds[*pfd_i].fd;

    if (nbytes <= 0) { // Got error or connection closed by client
        if (nbytes == 0) {
            // Connection closed
            std::cout << "pollserver: socket " << sender_fd << " hung up" << std::endl;
        } else {
            std::cerr << "recv";
        }

        close(pfds[*pfd_i].fd); // Bye!

        del_from_pfds(pfds, *pfd_i, fd_count);

        // reexamine the slot we just deleted
        (*pfd_i)--;

    } else { // We got some good data from a client
        printf("pollserver: recv from fd %d: %.*s", sender_fd,
                nbytes, buf);
        // Send to everyone!
        for(int j = 0; j < *fd_count; j++) {
            int dest_fd = pfds[j].fd;

            // Except the listener and ourselves
            if (dest_fd != listener && dest_fd != sender_fd) {
                if (send(dest_fd, buf, nbytes, 0) == -1) {
                    perror("send");
                }
            }
        }
    }
}

/*
 * Process all existing connections.
 */
void Server::process_connections(int listener, int *fd_count, int *fd_size,
        struct pollfd **pfds)
{
    for(int i = 0; i < *fd_count; i++) {

        // Check if someone's ready to read
        if ((*pfds)[i].revents & (POLLIN | POLLHUP)) {
            // We got one!!

            if ((*pfds)[i].fd == listener) {
                // If we're the listener, it's a new connection
                handle_new_connection(listener, fd_count, fd_size,
                        pfds);
            } else {
                // Otherwise we're just a regular client
                handle_client_data(listener, fd_count, *pfds, &i);
            }
        }
    }
}

void Server::addClient(const Client &new_client)
{
    clients.push_back(new_client);
}

void Server::removeClient(const std::string nickname)
{
    std::vector<Client>::iterator it = clients.begin();
    while (it != clients.end() && it->getNick() != nickname)
        it++;
    if (it != clients.end())
        clients.erase(it);
    else
        std::cerr << "client " << nickname << " doesn't exist." << std::endl;
}