#include "Server.hpp"

bool is_digit(const char *av)
{
    int i = 0;
    while (av[i])
    {
        if (av[i] < '0' || av[i] > '9')
            return false;
        i++;
    }
    return true;
}

bool is_in_charset(int carac, const std::string charset)
{
    for (size_t i = 0; i < charset.size(); i++)
        if (carac == charset[i])
            return true;
    return false;
}

bool nickname_in_list(const std::string nickname, std::vector<Client> clients)
{
    std::vector<Client>::iterator it = clients.begin();
    while (it != clients.end())
    {
        if (it->getNick() == nickname)
        {
            std::cerr << nickname << " already exists." << std::endl;
            return true;
        }
        it++;
    }
    return false;
}

bool valid_nickname(const std::string nickname)
{
    if (!nickname[0] || is_in_charset(nickname[0], "$:#&%+~") || nickname.size() > 9)
        return false;

    for (size_t i = 0; i < nickname.size(); i++)
        if (is_in_charset(nickname[i], " ,*?!@"))
            return false;
    return true;
}
