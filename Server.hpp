#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
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
#include <map>
#define PORT "6667"
#include "Channel.hpp"
#include "Client.hpp"

#define IRC "ircserver"
#define RPL_WELCOME "001"
#define RPL_YOURHOST "002"
#define RPL_CREATED "003"
#define RPL_MYINFO "004"
#define RPL_WHOISUSER "311"
#define RPL_WHOISSERVER "312"
#define RPL_WHOISOPERATOR "313"
#define RPL_ENDOFWHO "315"
#define RPL_WHOISIDLE "317"
#define RPL_ENDOFWHOIS "318"
#define RPL_WHOISCHANNELS "319"
#define RPL_CHANNELMODEIS "324"
#define RPL_NOTOPIC "331" 
#define RPL_TOPIC "332"
#define RPL_INVITING "341"
#define RPL_WHOREPLY "352"
#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"
#define RPL_YOUROPER "381"

#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHSERVER "402"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CANNOTSENDTOCHAN "404"
#define ERR_TOOMANYCHANNELS "405"
#define ERR_TOOMANYTARGETS "407"
#define ERR_NOORIGIN "409"
#define ERR_NORECIPIENT "411"
#define ERR_NOTEXTTOSEND "412"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_ERRONUSNICKNAME "432"
#define ERR_NICKNAMEINUSE "433"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTONCHANNEL "442"
#define ERR_USERONCHANNEL "443"
#define ERR_NOTREGISTERED "451"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTERED "462"
#define ERR_PASSWDMISMATCH "464"
#define ERR_CHANNELISFULL "471"
#define ERR_UNKNOWNMODE "472"
#define ERR_INVITEONLYCHAN "473"
#define ERR_BADCHANNELKEY "475"
#define ERR_BADCHANNELMASK "476"
#define ERR_NOPRIVILEGES "481"
#define ERR_CHANOPRIVSNEEDED "482"
#define ERR_CANTKILLSERVER "483"
#define ERR_NOOPERHOST "491"
#define ERR_UMODEUNKNOWNFLAG "501"
#define ERR_USERSDONTMATCH "502"



typedef struct Message
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string raw;
}Message;

class Server
{
    public :
        Server(std::string port, std::string password);
        ~Server();

        const char *inet_ntop2(void *addr, char *buf, size_t size);
        int         get_listener_socket(void);

        void add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count, int *fd_size);
        void del_from_pfds(struct pollfd pfds[], int i, int *fd_count);

        void handle_new_connection(int listener, int *fd_count, int *fd_size, struct pollfd **pfds);
        void handle_client_data(int listener, int *fd_count, struct pollfd *pfds, int *pfd_i);
        void process_connections(int listener, int *fd_count, int *fd_size, struct pollfd **pfds);
        

        void    cmdPass(Client *client, std::vector<std::string> params);
        void    cmdNick(Client *client, std::vector<std::string> params);
        void    cmdUser(Client *client, std::vector<std::string> params);
        void    cmdCap(Client *client, std::vector<std::string> params);
        void    cmdPing(Client *client, std::vector<std::string> params);
        void    cmdPong(Client *client, std::vector<std::string> params);
        void    cmdPrivMsg(Client *client, std::vector<std::string> params);
        void    cmdWhoIs(Client *client, std::vector<std::string> params);
        void    cmdNotice(Client *client, std::vector<std::string> params);
        void    cmdJoin(Client *client, std::vector<std::string> params);
        void    cmdKick(Client *client, std::vector<std::string> params);
        void    cmdInvite(Client *client, std::vector<std::string> params);
        void    cmdTopic(Client *client, std::vector<std::string> params);
        void    cmdOper(Client *client, std::vector<std::string> params);
        void    cmdQuit(Client *client, std::vector<std::string> params);
        void    cmdPart(Client *client, std::vector<std::string> params);
        void    cmdMode(Client *client, std::vector<std::string> params);
        void    cmdWho(Client *client, std::vector<std::string> params);
        void    modeStatus(Client *client, Channel *channel);
        void    modeApply(Client *client, Channel *channel, std::vector<std::string> &params);
        void    modeI(Client *client, Channel *channel, bool adding);
        void    modeT(Client *client, Channel *channel, bool adding);
        void    modeK(Client *client, Channel *channel, bool adding, std::vector<std::string> &params, int &arg_idx);
        void    modeO(Client *client, Channel *channel, bool adding, std::vector<std::string> &params, int &arg_idx);
        void    modeL(Client *client, Channel *channel, bool adding, std::vector<std::string> &params, int &arg_idx);
        void    sendNeedMoreParams(Client *client);

        std::string buildMessage(std::string prefix, std::string command, std::vector<std::string> params, std::string trailing);

        void    removeClient(Client *client);
        void    sendToChan(Channel *channel, std::string message, Client *exclude);
        void    sendToClient(Client *client, std::string message);
        Channel    *createChannel(Client *client, const std::string name);
        Channel     *findChannel(const std::string name);
        Client *findClient(std::vector<Client *> array, std::string name);
        void    sendUsersInChannel(Client *client, Channel *channel);
        void    broadcastNewNick(Client *client, std::string oldnick, std::string newnick);
        void    broadcast(Client *client, std::string message, std::string exclude);


        int listener;
        int fd_size;
        int fd_count;

        struct pollfd *pfds;
        std::vector<Client *> clients;
        std::vector<Channel *> channel_list;
        std::map<std::string, void(Server::*)(Client *, std::vector<std::string>)> _commands;

    private :
        std::string _name;
        std::string _port;
        std::string _server_password;
};

std::vector<Message> parseMessage(std::string data);

#endif