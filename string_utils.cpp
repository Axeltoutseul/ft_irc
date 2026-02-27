#include "ft_irc.hpp"

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
