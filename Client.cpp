#include "Client.hpp"

Client::Client(): _password_ok(false), _registered(false), _has_nick(false), _has_user(false) {}

Client::~Client() {}

void    Client::setFd(const int fd) {
    _fd = fd;
}

void    Client::setNick(const std::string nick) {
    _nick = nick;
}

void    Client::setUsername(const std::string username) {
    _username = username;
}

void    Client::setHostname(const std::string hostname) {
    _hostname = hostname;
}

void    Client::setServername(const std::string servername) {
    _servername = servername;
}

void    Client::setRealname(const std::string realname) {
    _realname = realname;
}

void    Client::setPfd(struct pollfd *pfd) {
    _pfd = pfd;
}

void    Client::setStatePass(const bool password_ok) {
    _password_ok = password_ok;
}

void    Client::setStateRegister(const bool registered) {
    _registered = registered;
}

void    Client::setStateHasNick(const bool has_nick) {
    _has_nick = has_nick;
}

void    Client::setStateHasUser(const bool has_user) {
    _has_user = has_user;
}

int           Client::getFd() {
    return (_fd);
}

const std::string   Client::getNick() {
    if (_nick.empty())
        return ("*");
    return (_nick);
}

const std::string   Client::getUsername() {
    return (_username);
}

const std::string   Client::getHostname() {
    return (_hostname);
}

const std::string   Client::getServername() {
    return (_servername);
}

const std::string   Client::getRealname() {
    return (_realname);
}

const std::string   Client::getBuffer() {
    return (_recv_buffer);
}

const struct pollfd   *Client::getPfd() {
    return (_pfd);
}

bool  Client::getStatePass() {
    return (_password_ok);
}
bool  Client::getStateRegister() {
    return (_registered);
}

bool Client::getStateHasNick() {
    return (_has_nick);
}

bool Client::getStateHasUser() {
    return (_has_user);
}

void Client::appendBuffer(std::string buf) {
    _recv_buffer += buf;
}

void Client::clearBuffer() {
    _recv_buffer.clear();
}

void    Client::buildPrefix() {
    prefix = _nick + "!" + _username + "@" + _hostname;
}

bool is_in_charset(int carac, const std::string charset)
{
    for (size_t i = 0; i < charset.size(); i++)
        if (carac == charset[i])
            return true;
    return false;
}

bool isNickInUse(const std::string nickname, int client_fd, std::vector<Client *> clients)
{
    std::vector<Client *>::iterator it = clients.begin();
    while (it != clients.end())
    {
        if ((*it)->getNick() == nickname && client_fd != (*it)->getFd())
            return true;
        it++;
    }
    return false;
}

bool isNickValid(const std::string nickname)
{
    if (!nickname[0] || is_in_charset(nickname[0], "$:#&%+~") || nickname.size() > 9) {
        return false;
    }
    if (!isalpha(nickname[0]))
        return false;
    for (size_t i = 0; i < nickname.size(); i++)
        if (is_in_charset(nickname[i], " ,*?!@:"))
            return false;
    return true;
}