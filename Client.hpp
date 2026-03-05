#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
    public :
        Client();
        ~Client();
    private :
        std::string _nick;
        std::string _user;

};

#endif