#include "Client.hpp"

Client::Client(): _password_ok(false), _registered(false) {}

Client::~Client() {}

void    Client::setNick(const std::string nick) {
    _nick = nick;
}

void    Client::setUser(const std::string user) {
    _user = user;
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

const std::string   Client::getNick() {
    return (_nick);
}

const std::string   Client::getUser() {
    return (_user);
}

const struct pollfd   *Client::getPfd() {
    return (_pfd);
}

bool  Client::getStatePass() const {
    return (_password_ok);
}
bool  Client::getStateRegister() const {
    return (_registered);
}