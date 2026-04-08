#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include <iostream>
#include <bits/stdc++.h>
#include <vector>

class Channel
{
    public :
        Channel(std::string name);
        ~Channel();
        std::string getName() const;
        void execCommand(const std::string command);
        bool is_in_list(const Client *client, std::vector<Client *> client_list);
        void removeFromChannel(const Client *client);

        std::vector<Client *> _operator;
        std::vector<Client *> _users;
        std::vector<Client *> _invite;
        std::vector<Client *> _voice;

        std::string            topic;
        std::string            key;
        bool                   inviteOnly;
        bool                   topicRestricted;
        int                    userLimit;

    private :

    std::string _name;
    std::string _type;
};

bool is_in_charset(int carac, const std::string charset);
bool isChannelNameValid(const std::string channel_name);

#endif