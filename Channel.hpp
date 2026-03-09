#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include <iostream>
#include <bits/stdc++.h>
#include <vector>

class Channel
{
    public :
        Channel();
        ~Channel();
        void execCommand(const std::string command);
        bool is_in_list(const std::string username);

        std::vector<Client> _operator;
        std::vector<Client> _users;
        std::vector<Client> _invite;
        std::vector<Client> _voice;
    private :
};

#endif