#include "Server.hpp"

Server::Server(std::string port, std::string password): _name("ircserver"), _port(port), _server_password(password) {

    // Set up and get a listening socket
    listener = get_listener_socket();
    if (listener == -1) {
        std::cerr << "error getting listening socket" << std::endl;
        exit(1);
    }
   _commands["PASS"] = &Server::cmdPass;
   _commands["NICK"] = &Server::cmdNick;
   _commands["USER"] = &Server::cmdUser;
   _commands["CAP"] = &Server::cmdCap;
   _commands["PING"] = &Server::cmdPing;
   _commands["PONG"] = &Server::cmdPong;
   _commands["PRIVMSG"] = &Server::cmdPrivMsg;
   _commands["WHOIS"] = &Server::cmdWhoIs;
   _commands["NOTICE"] = &Server::cmdNotice;
   _commands["JOIN"] = &Server::cmdJoin;
   _commands["KICK"] = &Server::cmdKick;
   _commands["INVITE"] = &Server::cmdInvite;
   _commands["TOPIC"] = &Server::cmdTopic;
   _commands["OPER"] = &Server::cmdOper;
   _commands["QUIT"] = &Server::cmdQuit;
   _commands["PART"] = &Server::cmdPart;
   _commands["MODE"] = &Server::cmdMode;
   _commands["WHO"] = &Server::cmdWho;

    fd_size = 5;
    fd_count = 0;
    pfds = new pollfd[fd_size];

    // Add the listener to set;
    // Report ready to read on incoming connection
    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1; // For the listener

    std::cout << "pollserver: waiting for connections..." << std::endl;
}

Server::~Server() {
    size_t i = 0;
    while (i < clients.size())
        delete clients[i++];
    i = 0;
    while (i < channel_list.size())
        delete channel_list[i++];
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
    if ((rv = getaddrinfo(NULL, _port.c_str(), &hints, &ai)) != 0) {
        fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
        return -1;
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
        freeaddrinfo(ai);
        return -1;
    }
    freeaddrinfo(ai); // All done with this
                      // Listen
    if (listen(listener, 10) == -1) {
        close(listener);
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
        *fd_size *= 2;
        pollfd *new_pfds = new pollfd[*fd_size];
        std::copy(*pfds, *pfds + *fd_count, new_pfds);
        delete[] *pfds;
        *pfds = new_pfds;
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
void Server::handle_new_connection(int listener, int *fd_count,
        int *fd_size, struct pollfd **pfds)
{
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    int newfd; // Newly accept()ed socket descriptor
    char remoteIP[INET6_ADDRSTRLEN];
    Client *new_client;

    addrlen = sizeof(remoteaddr);
    newfd = accept(listener, (struct sockaddr *)&remoteaddr,
            &addrlen);

    if (newfd == -1) {
        std::cerr << "accept failed" << std::endl;
    } else {
        add_to_pfds(pfds, newfd, fd_count, fd_size);

        new_client = new(Client);
        new_client->setFd(newfd);
        clients.push_back(new_client);

        printf("pollserver: new connection from %s on socket %d\n",
                inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP),
                newfd);
    }
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

    Client *sender_client = NULL;

    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i]->getFd() == sender_fd) {
            sender_client = clients[i];
            break;
        }
    }

    if (nbytes <= 0) { // Got error or connection closed by client
        if (nbytes == 0) {
            // Connection closed
            std::cout << "pollserver: socket " << sender_fd << " hung up" << std::endl;
        } else {
            std::cerr << "recv failed" << std::endl;
        }

        close(pfds[*pfd_i].fd); // Bye!
        del_from_pfds(pfds, *pfd_i, fd_count);

        if (sender_client)
            removeClient(sender_client);
        std::cout << "clients.size() = " << clients.size() << std::endl;

        // reexamine the slot we just deleted
        (*pfd_i)--;

    } else { // We got some good data from a client

        // add terminating bytes because recv() dont add it
        buf[nbytes] = '\0';

        // parse message into struct Message
        sender_client->appendBuffer(std::string(buf, nbytes));
        std::string buffer = sender_client->getBuffer();
        sender_client->clearBuffer();
        std::vector<Message> parsed_msg = parseMessage(buffer);
        size_t last_crlf = buffer.rfind("\r\n");
        if (last_crlf == std::string::npos)
            sender_client->appendBuffer(buffer); // no complete message yet
        else if (last_crlf + 2 < buffer.size())
            sender_client->appendBuffer(buffer.substr(last_crlf + 2));

        //call CMD function
        for (size_t i = 0; i < parsed_msg.size(); i++) {

            std::map<std::string, void(Server::*)(Client *, std::vector<std::string>)>::iterator it_cmd = _commands.find(parsed_msg[i].command);
            if (it_cmd != _commands.end()) {
                        (this->*(it_cmd->second))(sender_client, parsed_msg[i].params);
            }
            else {
                std::vector<std::string> tmp;
                tmp.push_back(sender_client->getNick());
                tmp.push_back(parsed_msg[i].command);
                sendToClient(sender_client, buildMessage(IRC, ERR_UNKNOWNCOMMAND, tmp, "Unknown Command"));
            }
        }

        std::vector<Client *>::iterator it = std::find(clients.begin(), clients.end(), sender_client);
        if (it == clients.end()) {
            close(pfds[*pfd_i].fd);
            del_from_pfds(pfds, *pfd_i, fd_count);
            (*pfd_i)--;
            return;
        }

        //Set registered and send Welcome message
        if (!sender_client->getStateRegister() && sender_client->getStateHasNick()
                    && sender_client->getStateHasUser() && sender_client->getStatePass()) {
            sender_client->setStateRegister(true);
            sendToClient(sender_client, buildMessage(IRC, RPL_WELCOME, std::vector<std::string>(1, sender_client->getNick()), "Welcome to the Internet Relay Network"));
            sender_client->buildPrefix();
            std::cout << "Welcome Message sent to "  << sender_client->getRealname() << " fd "  << sender_client->getFd() << std::endl; 
        }

        std::cout << "pollserver: recv from fd " << sender_fd << ": " << std::string(buf, nbytes) << std::endl;
        if (std::string(buf, nbytes).find("\r\n") == std::string::npos)
            std::cout << std::endl;
        (void)listener;
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

std::vector<Message>parseMessage(std::string data)
{
    std::vector<Message>messages;
    
    size_t pos;
    while ((pos = data.find("\r\n")) != std::string::npos)
    {
        Message tmp;
        void    sendJoinToChan(Client *client, Channel *channel);
        tmp.raw = data.substr(0, pos);
        messages.push_back(tmp);
        data.erase(0, pos + 2);
    }
    for (size_t i = 0; i < messages.size(); i++)
    {
        std::string msg = messages[i].raw;
        std::string trailing;
        pos = msg.find(" :");
        if (pos != std::string::npos) {
            trailing = msg.substr(pos + 2);
            msg = msg.substr(0, pos);
        }
        pos = msg.find(" ");
        messages[i].command = msg.substr(0, pos);
        msg.erase(0, pos + 1);
        
        std::stringstream ss(msg);
        std::string word;
        while (ss >> word)
        messages[i].params.push_back(word);
        if (!trailing.empty())
            messages[i].params.push_back(trailing);
    }
    
    return messages;
}

void    Server::sendToClient(Client *client, std::string message) {
    std::cout << "SEND >" << message << std::endl;
    if(send(client->getFd(), message.c_str(), message.size(), 0) == -1)
        std::cerr << "send failed" << std::endl;
}

void    Server::sendToChan(Channel *channel, std::string message, Client *exclude)
{
    std::vector<Client *>::iterator it = channel->_users.begin();
    while (it != channel->_users.end()) {
        if (!(exclude && (*it)->getNick() == exclude->getNick()))
            sendToClient((*it), message);
        it++;
    }
}

std::string Server::buildMessage(std::string prefix, std::string command, std::vector<std::string> params, std::string trailing)
{
    std::string message;
    message =  ":" + prefix + " " + command;
    for (size_t i = 0; i < params.size(); i++) {
        message += " " + params[i];
    }
    if (!trailing.empty())
        message += " :" + trailing;
    message += "\r\n";
    return (message);
}

void    Server::removeClient(Client *client)
{
    std::vector<Channel *>::iterator it = channel_list.begin();
    std::vector<Client *>::iterator it2 = clients.begin();

    while (it != channel_list.end())
    {
        if ((*it)->is_in_list(client, (*it)->_users))
            (*it)->removeFromChannel(client);
        if ((*it)->_users.size() == 0)
        {
            delete *it;
            it = channel_list.erase(it);
        }
        else
            it++;
    }
    while (it2 != clients.end() && *it2 != client)
        it2++;
    if (it2 != clients.end())
        clients.erase(it2);
    delete client;
}

Channel *Server::findChannel(const std::string name)
{
    std::vector<Channel *>::iterator it = channel_list.begin();
    while (it != channel_list.end())
    {
        if ((*it)->getName() == name)
            return (*it);
        it++;
    }
    return (NULL);
}

Client *Server::findClient(std::vector<Client *> array, std::string name) {
    std::vector<Client *>::iterator it = array.begin();
    while (it != array.end()) {
        if ((*it)->getNick() == name)
            return (*it);
        it++;
    }
    return (NULL);
}

void    Server::broadcastNewNick(Client *client, std::string oldnick, std::string newnick)
{
    std::vector<Client *> warned;
    for (size_t i = 0; i < channel_list.size(); i++) {
        if (findClient(channel_list[i]->_users, oldnick)) {
            for (size_t j = 0; j < channel_list[i]->_users.size(); j++) {
                if (!findClient(warned, channel_list[i]->_users[j]->getNick())) {
                    sendToClient(channel_list[i]->_users[j], buildMessage(client->prefix, "NICK", std::vector<std::string>(1, newnick), ""));
                    warned.push_back(channel_list[i]->_users[j]);
                }
            }
        }
    }
}

void    Server::broadcast(Client *client, std::string message, std::string exclude)
{
    std::vector<Client *> warned;
    for (size_t i = 0; i < channel_list.size(); i++) {
        if (findClient(channel_list[i]->_users, client->getNick())) {
            for (size_t j = 0; j < channel_list[i]->_users.size(); j++) {
                if (!findClient(warned, channel_list[i]->_users[j]->getNick()) && channel_list[i]->_users[j]->getNick() != exclude) {
                    sendToClient(channel_list[i]->_users[j], message);
                    warned.push_back(channel_list[i]->_users[j]);
                }
            }
        }
    }
}
