#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "utils.hpp"

class Client
{
    public :
        Client();
        ~Client();

        void    setFd(const int fd);
        void    setNick(const std::string nick);
        void    setUser(const std::string user);
        void    setPfd(struct pollfd *pfd);
        void    setStatePass(const bool password_ok);
        void    setStateRegister(const bool registered);
        
        int   getFd();
        const std::string getNick();
        const std::string getUser();
        const struct pollfd         *getPfd();
        bool        getStatePass();
        bool        getStateRegister();


        bool handleClientInfo(std::string msg);

    private :
        int                     _fd;
        std::string             _nick;
        std::string             _user;
        struct pollfd           *_pfd;
        bool                    _password_ok;
        bool                    _registered;

};

#endif