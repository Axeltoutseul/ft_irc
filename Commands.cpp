#include "Server.hpp"

//Send by client on first connexion, ignore it
void    Server::cmdCap(Client *client, std::vector<std::string> params) {
    (void)client;
    (void)params;
}

//Set Nickname
void    Server::cmdNick(Client *client, std::vector<std::string> params) {
    if (!isNickValid(params[0])) {
        std::cerr << "NICK is not valid" << std::endl;
        sendToClient(client, buildMessage(IRC, ERR_ERRONUSNICKNAME, params, "Erroneous Nickname"));
        return ; 
    }
    if (isNickInUse(params[0], client->getFd(), clients)) {
        sendToClient(client, buildMessage(IRC, ERR_NICKNAMEINUSE, params, "Nickname is already in use"));
        return ;
    }
    if (client->getNick() != "*") {
        broadcastNewNick(client, client->getNick(), params[0]);
    }
    client->setNick(params[0]);
    if (!client->getStateHasNick())
        client->setStateHasNick(true);
}

//See if it match with server password 
void    Server::cmdPass(Client *client, std::vector<std::string> params) {
    if (client->getStateRegister() == true)
    {
        sendToClient(client, buildMessage(IRC, ERR_ALREADYREGISTERED, std::vector<std::string>(1, client->getNick()), "You are already registered"));
        return ;
    }
    if (this->_server_password == params[0])
        client->setStatePass(true);
    else
        sendToClient(client, buildMessage(IRC, ERR_PASSWDMISMATCH, params, "Password incorrect"));
}

//Set User <user><hostname><servername>:<realname>
void    Server::cmdUser(Client *client, std::vector<std::string> params) {
    if (params.size() != 4) {
        std::cerr << "You must have 4 arguments." << std::endl;
        return ;
    }
    client->setUsername(params[0]);
    client->setHostname(params[1]);
    client->setServername(params[2]);
    client->setRealname(params[3]);
    client->setStateHasUser(true);
}

//If client send PING answer with PONG
void    Server::cmdPing(Client *client, std::vector<std::string> params) {
    if (params.empty()) {
        sendToClient(client, buildMessage(IRC, ERR_NOORIGIN, std::vector<std::string>(1, client->getNick()), "No origin specified"));
        return ;
    }
    sendToClient(client, buildMessage(IRC, "PONG", std::vector<std::string>(), params[0]));
}

//
void    Server::cmdPong(Client *client, std::vector<std::string> params) {
    (void)params;
    (void)client;
}

//Send message to a target : PRIVMSG <target>:<text>
void    Server::cmdPrivMsg(Client *client, std::vector<std::string> params) {
    if (!client->getStateRegister()) {
        sendToClient(client, buildMessage(IRC, ERR_NOTREGISTERED, std::vector<std::string>(1, client->getNick()), "You are not registered"));
        return ;
    }
    if (params.size() < 2) {
        sendToClient(client, buildMessage(IRC, ERR_NOTEXTTOSEND, std::vector<std::string>(), "No text to send"));
        return ;
    }
    Client *target = findClient(clients, params[0]);
    if (target) {
        sendToClient(target, buildMessage(client->prefix, "PRIVMSG", std::vector<std::string>(1, target->getNick()), params[1]));
        return ;
    }
    Channel *chan = findChannel(params[0]);
    if (chan) {
        if (findClient(chan->_users, client->getNick()))
            sendToChan(chan, buildMessage(client->prefix, "PRIVMSG", std::vector<std::string>(1, chan->getName()), params[1]), client);
        else
            sendToClient(client, buildMessage(IRC, ERR_CANNOTSENDTOCHAN, std::vector<std::string>(1, chan->getName()), "Cannot send to channel"));
        return ;
    }
    sendToClient(client, buildMessage(IRC, ERR_NOSUCHNICK, std::vector<std::string>(1, params[0]), "No such nick/channel"));
}

void    Server::cmdNotice(Client *client, std::vector<std::string> params) {
    if (!client->getStateRegister()) {
        sendToClient(client, buildMessage(IRC, ERR_NOTREGISTERED, std::vector<std::string>(1, client->getNick()), "You are not registered"));
        return ;
    }
    if (params.size() < 2)
        return ;
    Client *receiver = findClient(clients, params[0]);
    if (receiver) {
        sendToClient(receiver, buildMessage(client->prefix, "NOTICE", std::vector<std::string>(1, receiver->getNick()), params[1]));
        return ;
    }
    Channel * chan = findChannel(params[0]);
    if (chan)
        sendToChan(chan, buildMessage(client->prefix, "NOTICE", std::vector<std::string>(1, chan->getName()), params[1]), client);
}

void    Server::cmdWho(Client *client, std::vector<std::string> params) {
    if (params.size() < 1)
        return ;
    Channel *chan = findChannel(params[0]);
    if (chan) {
        for (size_t i = 0; i < chan->_users.size(); i++) {
            Client *user = chan->_users[i];
            std::vector<std::string> tmp;
            tmp.push_back(client->getNick());
            tmp.push_back(chan->getName());
            tmp.push_back(user->getUsername());
            tmp.push_back(user->getHostname());
            tmp.push_back(user->getServername());
            tmp.push_back(user->getNick());
            tmp.push_back("H");
            sendToClient(client, buildMessage(IRC, RPL_WHOREPLY, tmp, "0 " + user->getRealname()));
        }
        sendToClient(client, buildMessage(IRC, RPL_ENDOFWHO, std::vector<std::string>(1, chan->getName()), "End of WHO list"));
        return ;
    }
    Client *user = findClient(clients, params[0]);
    if (user) {
        std::vector<std::string> tmp;
        tmp.push_back(client->getNick());
        tmp.push_back("*");
        tmp.push_back(user->getUsername());
        tmp.push_back(user->getHostname());
        tmp.push_back(user->getServername());
        tmp.push_back(user->getNick());
        tmp.push_back("H");
        sendToClient(client, buildMessage(IRC, RPL_WHOREPLY, tmp, "0 " + user->getRealname()));
        sendToClient(client, buildMessage(IRC, RPL_ENDOFWHO, std::vector<std::string>(1, user->getNick()), "End of WHO list"));
        return ;
    }
    sendToClient(client, buildMessage(IRC, RPL_ENDOFWHO, std::vector<std::string>(1, params[0]), "End of WHO list"));
}


// WHOIS cmd need to be answered with RPL_WHOISUSER, RPL_WHOISSERVER and RPL_ENDOFWHOIS
void    Server::cmdWhoIs(Client *client, std::vector<std::string> params) {

    if (!client->getStateRegister()) {
        sendToClient(client, buildMessage(IRC, ERR_NOTREGISTERED, std::vector<std::string>(1, client->getNick()), "You are not registered"));
        return ;
    }
    Client *user = findClient(clients, params[0]);
    if (!user) {
        sendToClient(client, buildMessage(IRC, ERR_NOSUCHNICK, std::vector<std::string>(1, params[0]), "No such nick/channel"));
        return ;
    }
    std::vector<std::string> params_user;
    params_user.push_back(user->getNick());
    params_user.push_back(user->getUsername());
    params_user.push_back(user->getHostname());
    sendToClient(client, buildMessage(IRC, RPL_WHOISUSER, params_user, user->getRealname()));
    std::vector<std::string> params_server;
    params_server.push_back(user->getNick());
    params_server.push_back(_name);
    sendToClient(client, buildMessage(IRC, RPL_WHOISSERVER, params_server, "Simple IRC server"));
    params_server.erase(params_server.end());
    sendToClient(client, buildMessage(IRC, RPL_ENDOFWHOIS, params_server, "End of WHOIS list"));
}

void    Server::cmdJoin(Client *client, std::vector<std::string> params)
{
    if (!client->getStateRegister()) {
        sendToClient(client, buildMessage(IRC, ERR_NOTREGISTERED, std::vector<std::string>(1, client->getNick()), "You are not registered"));
        return ;
    }
    if (!isChannelNameValid(params[0])) {
        std::vector<std::string> tmp; tmp.push_back(client->getNick()); tmp.push_back(params[0]);
        sendToClient(client, buildMessage(IRC, ERR_BADCHANNELMASK, tmp, "Bad Channel Mask"));
        return ;
    }
    Channel *channel;
    if (!findChannel(params[0])) {
        channel = createChannel(client, params[0]);
    }
    else {
        channel = findChannel(params[0]);
        if (findClient(channel->_users, client->getNick()))
            return ;
        if (channel->inviteOnly && !findClient(channel->_invite, client->getNick())) {
            std::vector<std::string> tmp; tmp.push_back(client->getNick()); tmp.push_back(channel->getName());
            sendToClient(client, buildMessage(IRC, ERR_INVITEONLYCHAN, tmp, "Cannot join channel (+i)"));
            return ;
        }
        if (!channel->key.empty() && channel->key != params[1]) {
            std::vector<std::string> tmp; tmp.push_back(client->getNick()); tmp.push_back(channel->getName());
            sendToClient(client, buildMessage(IRC, ERR_BADCHANNELKEY, tmp, "Cannot join channel (+k)"));
            return ;
        }
        if (channel->userLimit > 0 && (int)channel->_users.size() >= channel->userLimit) {
            std::vector<std::string> tmp; tmp.push_back(client->getNick()); tmp.push_back(channel->getName());
            sendToClient(client, buildMessage(IRC, ERR_CHANNELISFULL, tmp, "Cannot join channel (+l)"));
            return ;
        }
        channel->_users.push_back(client);
    }
    sendToChan(channel, buildMessage(client->prefix, "JOIN", std::vector<std::string>(1, channel->getName()), std::string()), NULL);
    std::vector<std::string> tmp; tmp.push_back(client->getNick()); tmp.push_back(channel->getName());
    if (!channel->topic.empty())
        sendToClient(client, buildMessage(IRC, RPL_TOPIC, tmp, channel->topic));
    else
        sendToClient(client, buildMessage(IRC, RPL_NOTOPIC, tmp, "No topic is set"));
    sendUsersInChannel(client, channel);
}

void    Server::cmdKick(Client *client, std::vector<std::string> params)
{
    if (!client->getStateRegister()) {
        sendToClient(client, buildMessage(IRC, ERR_NOTREGISTERED, std::vector<std::string>(1, client->getNick()), "You are not registered"));
        return ;
    }
    if (params.size() < 1) {
        sendToClient(client, buildMessage(IRC, ERR_NEEDMOREPARAMS, std::vector<std::string>(1, "KICK"), "Not enough parameters"));
        return ;
    }
    Channel *channel = findChannel(params[0]);
    if (!channel) {
        sendToClient(client, buildMessage(IRC, ERR_NOSUCHSERVER, std::vector<std::string>(1, "KICK"), "No such channel"));
        return;
    }
    if (!findClient(channel->_operator, client->getNick())) {
        sendToClient(client ,buildMessage(IRC, ERR_CHANOPRIVSNEEDED, std::vector<std::string>(1, channel->getName()), "You're not channel operator"));
        return ;
    }
    Client *kick = findClient(channel->_users, params[1]);
    if (!kick) {
        std::vector<std::string> tmp;
        tmp.push_back(params[1]);
        tmp.push_back(channel->getName());
        sendToClient(client, buildMessage(IRC, ERR_USERNOTINCHANNEL, tmp, "They aren't on that channel"));
        return ;
    }
    std::string message = params[1];
    if (params.size() < 2)
        message += " :" + params[2];
    sendToChan(channel, buildMessage(client->prefix, "KICK", std::vector<std::string>(1, channel->getName()), message), NULL);
    std::vector<Client *>::iterator it = channel->_users.begin();
    while ((*it)->getNick() != kick->getNick())
        it++;
    channel->_users.erase(it);
}

void    Server::cmdInvite(Client *client, std::vector<std::string> params)
{
    if (!client->getStateRegister()) {
        sendToClient(client, buildMessage(IRC, ERR_NOTREGISTERED, std::vector<std::string>(1, client->getNick()), "You are not registered"));
        return ;
    }
    if (params.size() < 2) {
        sendToClient(client, buildMessage(IRC, ERR_NEEDMOREPARAMS, std::vector<std::string>(1, "INVITE"), "Not enough parameters"));
        return;
    }

    std::string target_nick = params[0];
    std::string chan_name   = params[1];

    Channel *chan = findChannel(chan_name);

    if (!chan) {
        sendToClient(client, buildMessage(IRC, ERR_NOSUCHCHANNEL, std::vector<std::string>(1, chan_name), "No such channel"));
        return;
    }
    if (!chan->is_in_list(client, chan->_users)) {
        sendToClient(client, buildMessage(IRC, ERR_NOTONCHANNEL, std::vector<std::string>(1, chan_name), "You're not on that channel"));
        return;
    }
    if (chan->inviteOnly && !chan->is_in_list(client, chan->_operator)) {
        sendToClient(client, buildMessage(IRC, ERR_CHANOPRIVSNEEDED, std::vector<std::string>(1, chan_name), "You're not channel operator"));
        return;
    }

    Client *target = findClient(clients, target_nick);

    if (!target) {
        sendToClient(client, buildMessage(IRC, ERR_NOSUCHNICK, std::vector<std::string>(1, target_nick), "No such nick"));
        return;
    }
    if (findClient(chan->_users, target_nick)) {
        std::vector<std::string> tmp; tmp.push_back(target_nick); tmp.push_back(chan_name);
        sendToClient(client, buildMessage(IRC, ERR_USERONCHANNEL, tmp, "is already on channel"));
        return ;
    }

    chan->_invite.push_back(target);

    std::vector<std::string> tmp; tmp.push_back(client->getNick()); tmp.push_back(target_nick); tmp.push_back(chan_name);
    sendToClient(client, buildMessage(IRC, RPL_INVITING, tmp, ""));
    sendToClient(target, buildMessage(client->prefix, "INVITE", std::vector<std::string>(1, target_nick), chan_name));
}


void    Server::cmdTopic(Client *client, std::vector<std::string> params)
{
    if (!client->getStateRegister()) {
        sendToClient(client, buildMessage(IRC, ERR_NOTREGISTERED, std::vector<std::string>(1, client->getNick()), "You are not registered"));
        return ;
    }
    if (params.empty()) {
        sendToClient(client, buildMessage(IRC, ERR_NEEDMOREPARAMS, std::vector<std::string>(1, "TOPIC"), "Not enough parameters"));
        return;
    }

    std::string chan_name = params[0];

    Channel *chan = findChannel(chan_name);
   
    if (!chan) {
        sendToClient(client, buildMessage(IRC, ERR_NOSUCHCHANNEL, std::vector<std::string>(1, chan_name), "No such channel"));
        return;
    }
    if (!chan->is_in_list(client, chan->_users)) {
        sendToClient(client, buildMessage(IRC, ERR_NOTONCHANNEL, std::vector<std::string>(1, chan_name), "You're not on that channel"));
        return;
    }
    if (params.size() < 2) {
        if (chan->topic.empty()) {
            sendToClient(client, buildMessage(IRC, RPL_NOTOPIC, std::vector<std::string>(1, chan_name), "No topic is set"));
        }
        else {
            sendToClient(client, buildMessage(IRC, RPL_TOPIC, std::vector<std::string>(1, chan_name), chan->topic));
        }
        return;
    }
    if (chan->topicRestricted && !findClient(chan->_operator, client->getNick())) {
        sendToClient(client, buildMessage(IRC, ERR_CHANOPRIVSNEEDED, std::vector<std::string>(1, chan_name), "You're not channel operator"));
        return ;
    }
    chan->topic = params[1];
    sendToChan(chan, buildMessage(client->prefix, "TOPIC", std::vector<std::string>(1, chan_name), chan->topic), NULL);
}

void    Server::cmdOper(Client *client, std::vector<std::string> params)
{
    Channel *chan = findChannel(params[0]);
    if (!client->getStateRegister()) {
        sendToClient(client, buildMessage(IRC, ERR_NOTREGISTERED, std::vector<std::string>(1, client->getNick()), "You are not registered"));
        return ;
    }
    if (!chan->key.empty() && params.size() < 2) {
        sendToClient(client, buildMessage(IRC, ERR_NEEDMOREPARAMS, std::vector<std::string>(1, "OPER"), "Not enough parameters"));
        return ;
    }
    if (!chan->key.empty() && params[1] != chan->key) {
        sendToClient(client, buildMessage(IRC, ERR_PASSWDMISMATCH, std::vector<std::string>(1, "OPER"), "Password incorrect"));
        return ;
    }
    chan->_operator.push_back(client);
    std::cout << "operator.size() = " << chan->_operator.size() << std::endl;
    sendToClient(findClient(clients, client->getNick()), buildMessage(IRC, RPL_YOUROPER, std::vector<std::string>(1, params[0]), "Your are now operator"));
}

void    Server::cmdQuit(Client *client, std::vector<std::string> params)
{
    if (params.size())
        broadcast(client, buildMessage(client->prefix, "QUIT", std::vector<std::string>(), params[0]), client->getNick());
    for (int i = 0; i < fd_count; i++) {
        if (client->getFd() == pfds[i].fd) {
            close(pfds[i].fd);
            del_from_pfds(pfds, i, &fd_count);
            break ;
        }
    }
    removeClient(client);
}

void    Server::cmdPart(Client *client, std::vector<std::string> params)
{
    if (!client->getStateRegister()) {
        sendToClient(client, buildMessage(IRC, ERR_NOTREGISTERED, std::vector<std::string>(1, client->getNick()), "You are not registered"));
        return ;
    }
    if (params.size() < 1) {
        sendToClient(client, buildMessage(IRC, ERR_NEEDMOREPARAMS, std::vector<std::string>(1, "PART"), "Not enough parameters"));
        return ;
    }
    std::vector<std::string> channels;
    std::string param = params[0];
    size_t pos = param.find(',');
    while (pos != std::string::npos) {
        channels.push_back(param.substr(0, pos));
        param.erase(0, pos + 1);
        pos = param.find(',');
    }
    channels.push_back(param);
    Channel *chan ;
    for (size_t i = 0; i < channels.size(); i++)
    {
        chan = findChannel(channels[i]);
        if (chan)
        {
            if (findClient(chan->_users, client->getNick())) {
                std::string reason;
                if (params.size() > 1)
                    reason = params[1];
                else
                    reason = "";
                sendToChan(chan, buildMessage(client->prefix, "PART", std::vector<std::string>(1, channels[i]), reason), NULL);
                chan->removeFromChannel(client);
            }
            else 
                sendToClient(client, buildMessage(IRC, ERR_NOTONCHANNEL, std::vector<std::string>(1, channels[i]), "You're not on that channel"));
        }
        else
            sendToClient(client, buildMessage(IRC, ERR_NOSUCHCHANNEL, std::vector<std::string>(1, channels[i]), "No such channel"));
    }
}

Channel    *Server::createChannel(Client *client, std::string name)
{
    Channel *new_channel = new Channel(name);
    channel_list.push_back(new_channel);
    new_channel->_users.push_back(client);
    new_channel->_operator.push_back(client);
    return (new_channel);
}

void    Server::sendUsersInChannel(Client *client, Channel *channel)
{
    std::vector<Client *>::iterator it = channel->_users.begin();
    std::string list;
    while (it != channel->_users.end()) {
        if (findClient(channel->_operator, (*it)->getNick()))
            list += "@";
        list += (*it)->getNick();
        it++;
        if (it != channel->_users.end())
            list += " ";
    }
    std::vector<std::string> params;
    params.push_back(client->getNick());
    params.push_back("=");
    params.push_back(channel->getName());
    sendToClient(client, buildMessage(IRC, RPL_NAMREPLY, params, list));
    params.erase(params.begin());
    sendToClient(client, buildMessage(IRC, RPL_ENDOFNAMES, params, "End of NAMES list"));
}