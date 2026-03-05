#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include <vector>

class Channel
{
    public :
        Channel();
        ~Channel();

        std::vector<Client *> _operator;
        std::vector<Client *> _users;
        std::vector<Client *> _invite;
        std::vector<Client *> _voice;
    private :
};

#endif