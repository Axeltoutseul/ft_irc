#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
    public :
        Client();
        ~Client();

        void    setNick(const std::string nick);
        void    setUser(const std::string user);
        void    setPfd(struct pollfd *pfd);
        void    setStatePass(const bool password_ok);
        void    setStateRegister(const bool registered);
        
        const std::string getNick();
        const std::string getUser();
        const struct pollfd         *getPfd();
        bool        getStatePass() const;
        bool        getStateRegister() const;

    private :
        std::string             _nick;
        std::string             _user;
        struct pollfd           *_pfd;
        bool                    _password_ok;
        bool                    _registered;

};

#endif