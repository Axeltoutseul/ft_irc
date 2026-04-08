#include "Channel.hpp"

Channel::Channel(std::string name) : 
    topic(""), inviteOnly(false), topicRestricted(false), userLimit(0), _name(name) {}

Channel::~Channel() {}

std::string Channel::getName() const {
    return _name;
}

bool Channel::is_in_list(const Client *client, std::vector<Client *> client_list)
{
    std::vector<Client *>::iterator it = client_list.begin();
    while (it != client_list.end() && *it != client)
        it++;
    if (it != client_list.end())
        return true;
    return false;
}

void Channel::removeFromChannel(const Client *client)
{
    std::vector<Client *>::iterator it = _users.begin();
    std::vector<Client *>::iterator it2 = _operator.begin();
    std::vector<Client *>::iterator it3 = _voice.begin();
    while (it != _users.end() && *it != client)
        it++;
    if (it != _users.end())
    {
        while (it2 != _operator.end() && *it2 != client)
            it2++;
        while (it3 != _voice.end() && *it3 != client)
            it3++;
        if (it2 != _operator.end())
            _operator.erase(it2);
        if (it3 != _voice.end())
            _voice.erase(it3);
        _users.erase(it);
    }
}

bool isChannelNameValid(const std::string channel_name)
{
    if (!is_in_charset(channel_name[0], "&#+!"))
        return false;
    for (size_t i = 1; i < channel_name.size(); i++)
        if (channel_name[i] == ' ' || channel_name[i] == 7 || channel_name[i] == ',' || channel_name[i] == ':' || i > 50)
            return false;
    return true;
}