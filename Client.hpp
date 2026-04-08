#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client
{
    public :
        Client();
        ~Client();

        std::string prefix;

        void    setFd(const int fd);
        void    setPfd(struct pollfd *pfd);
        void    setNick(const std::string nick);
        void    setUsername(const std::string username);
        void    setHostname(const std::string hostname);
        void    setServername(const std::string servername);
        void    setRealname(const std::string realname);
        void    setStatePass(const bool password_ok);
        void    setStateRegister(const bool registered);
        void    setStateHasNick(const bool has_nick);
        void    setStateHasUser(const bool has_user);
        void    appendBuffer(std::string buf);
        void    clearBuffer();

        int   getFd();
        const std::string           getNick();
        const std::string           getUsername();
        const std::string           getHostname();
        const std::string           getRealname();
        const std::string           getServername();
        const std::string           getBuffer();
        const struct pollfd         *getPfd();
        bool                        getStatePass();
        bool                        getStateRegister();
        bool                        getStateHasNick();
        bool                        getStateHasUser();


        bool    handleClientInfo(std::string msg);
        void    buildPrefix();

    private :
        int                     _fd;
        std::string             _nick;
        std::string             _username;
        std::string             _hostname;
        std::string             _servername;
        std::string             _realname;
        std::string             _recv_buffer;
        struct pollfd           *_pfd;
        bool                    _password_ok;
        bool                    _registered;
        bool                    _has_nick;
        bool                    _has_user;
};

bool isNickValid(const std::string nickname);
bool isNickInUse(const std::string nickname, int client_fd, std::vector<Client *> clients);

#endif