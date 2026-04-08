#include "Server.hpp"

void    Server::cmdMode(Client *client, std::vector<std::string> params)
{
    // MODE #channel [+/-flags] [args...]
    if (params.size() < 1) {
        sendNeedMoreParams(client);
        return;
    }

    Channel *channel = findChannel(params[0]);
    if (!channel) {
        std::vector<std::string> tmp;
        tmp.push_back(params[0]);
        sendToClient(client, buildMessage(IRC, ERR_NOSUCHCHANNEL, tmp, "No such channel"));
        return;
    }

    // MODE #channel sans flag : renvoyer le mode actuel
    if (params.size() == 1) {
        modeStatus(client, channel);
        return;
    }
    // Vérifier que le client est op sur ce channel
    if (!findClient(channel->_operator, client->getNick())) {
        std::vector<std::string> tmp;
        tmp.push_back(client->getNick());
        tmp.push_back(channel->getName());
        sendToClient(client, buildMessage(IRC, ERR_CHANOPRIVSNEEDED, tmp, "You're not channel operator"));
        return;
    }
    // MODE avec options
    modeApply(client, channel, params);
}

void    Server::modeStatus(Client *client, Channel *channel){
    std::string modestr = "+";
    if (channel->inviteOnly)
        modestr += "i";
    if (channel->topicRestricted)
        modestr += "t";
    if (!channel->key.empty())
        modestr += "k";
    if (channel->userLimit > 0)
        modestr += "l";
    std::vector<std::string> p;
    p.push_back(client->getNick());
    p.push_back(channel->getName());
    p.push_back(modestr);
    if (channel->userLimit > 0) {
        std::ostringstream oss;
        oss << channel->userLimit;
        p.push_back(oss.str());
    }
    sendToClient(client, buildMessage(IRC, RPL_CHANNELMODEIS, p, ""));
}

void    Server::modeApply(Client *client, Channel *channel, std::vector<std::string> &params) {
    std::string modestring = params[1];
    int         arg_idx = 2; // index du prochain argument dans params
    bool        adding = true; // + ou -

    for (size_t i = 0; i < modestring.size(); i++)
    {
        char c = modestring[i];
        if (c == '+') {
            adding = true;
            continue;
        }
        if (c == '-') {
            adding = false;
            continue;
        }

        if (c == 'i')
            modeI(client, channel, adding);
        else if (c == 't')
            modeT(client, channel, adding);
        else if (c == 'k')
            modeK(client, channel, adding, params, arg_idx);
        else if (c == 'o')
            modeO(client, channel, adding, params, arg_idx);
        else if (c == 'l')
            modeL(client, channel, adding, params, arg_idx);
        else {
            std::vector<std::string> tmp;
            tmp.push_back(std::string(1, c));
            sendToClient(client, buildMessage(IRC, ERR_UNKNOWNMODE, tmp, "is unknown mode char to me"));
        }    
    }
}

void    Server::modeI(Client *client, Channel *channel, bool adding) {
    channel->inviteOnly = adding;
    std::vector<std::string> tmp;
    tmp.push_back(channel->getName());
    tmp.push_back((adding ? "+i" : "-i"));
    sendToChan(channel, buildMessage(client->prefix, "MODE", tmp, ""), NULL);
}

void    Server::modeT(Client *client, Channel *channel, bool adding) {
    channel->topicRestricted = adding;
    std::vector<std::string> tmp;
    tmp.push_back(channel->getName());
    tmp.push_back(adding ? "+t" : "-t");
    sendToChan(channel, buildMessage(client->prefix, "MODE", tmp, ""), NULL);
}

void    Server::sendNeedMoreParams(Client *client) {
    std::vector<std::string> tmp;
    tmp.push_back("MODE");
    sendToClient(client, buildMessage(IRC, ERR_NEEDMOREPARAMS, tmp, "Not enough parameters"));
}

void    Server::modeK(Client *client, Channel *channel, bool adding, std::vector<std::string> &params, int &arg_idx) {
    if (adding) {
        if (arg_idx >= (int)params.size()) {
            sendNeedMoreParams(client);
            return;
        }
        channel->key = params[arg_idx++];
        std::vector<std::string> tmp;
        tmp.push_back(channel->getName());  
        tmp.push_back("+k");
        tmp.push_back(channel->key);
        sendToChan(channel, buildMessage(client->prefix, "MODE", tmp, ""), NULL);
    }
    else {
        channel->key = "";
        std::vector<std::string> tmp;
        tmp.push_back(channel->getName());
        tmp.push_back("-k");
        sendToChan(channel, buildMessage(client->prefix, "MODE", tmp, ""), NULL);
    }
}

void    Server::modeO(Client *client, Channel *channel, bool adding, std::vector<std::string> &params, int &arg_idx) {
    if (arg_idx >= (int)params.size()) {
        sendNeedMoreParams(client);
        return;
    }
    std::string target_nick = params[arg_idx++];
    Client *target = findClient(channel->_users, target_nick);
    if (!target) {
        std::vector<std::string> tmp;
        tmp.push_back(target_nick);
        tmp.push_back(channel->getName());
        sendToClient(client, buildMessage(IRC, ERR_USERNOTINCHANNEL, tmp, "They aren't on that channel"));
        return; 
    }
    if (adding) {
        if (!findClient(channel->_operator, target_nick))
            channel->_operator.push_back(target);
    }
    else {
        std::vector<Client *>::iterator it = channel->_operator.begin();
        while (it != channel->_operator.end() && (*it)->getNick() != target_nick)
            it++;
        if (it != channel->_operator.end())
            channel->_operator.erase(it);
    }
    std::vector<std::string> tmp; 
    tmp.push_back(channel->getName()); 
    tmp.push_back((adding ? "+o" : "-o"));
    tmp.push_back(target_nick);
    sendToChan(channel, buildMessage(client->prefix, "MODE", tmp, ""), NULL);
}

void    Server::modeL(Client *client, Channel *channel, bool adding, std::vector<std::string> &params, int &arg_idx) {
    if (adding) {
        if (arg_idx >= (int)params.size()) {
            sendNeedMoreParams(client);
            return;
        }
        channel->userLimit = std::atoi(params[arg_idx++].c_str());
        if (channel->userLimit <= 0) {
            channel->userLimit = 0;
            std::vector<std::string> tmp;
            tmp.push_back("l");
            sendToClient(client, buildMessage(IRC, ERR_UNKNOWNMODE, tmp, "Invalid mode parameter"));
            return;
        }
        std::ostringstream oss;
        oss << channel->userLimit;
        std::vector<std::string> tmp;
        tmp.push_back(channel->getName());
        tmp.push_back("+l");
        tmp.push_back(oss.str());
        sendToChan(channel, buildMessage(client->prefix, "MODE", tmp, ""), NULL);
    }
    else {
        channel->userLimit = 0;
        std::vector<std::string> tmp;
        tmp.push_back(channel->getName());
        tmp.push_back("-l");
        sendToChan(channel, buildMessage(client->prefix, "MODE", tmp, ""), NULL);
    }
}

