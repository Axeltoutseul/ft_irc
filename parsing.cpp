#include "Server.hpp"

int is_digit(const char *av)
{
    int i = 0;
    while (av[i])
    {
        if (av[i] < '0' || av[i] > '9')
            return 0;
        i++;
    }
    return (1);
}

int is_in_charset(int carac, const std::string charset)
{
    for (size_t i = 0; i < charset.size(); i++)
        if (carac == charset[i])
            return 1;
    return 0;
}

int valid_nickname(const std::string nickname)
{
    if (!nickname[0] || is_in_charset(nickname[0], "$:#&%+~"))
        return 0;

    for (size_t i = 0; i < nickname.size(); i++)
        if (is_in_charset(nickname[i], " ,*?!@"))
            return 0;
    return 1;
}
