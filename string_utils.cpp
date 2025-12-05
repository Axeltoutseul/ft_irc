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

int count_digits(const char *av)
{
    int i = 0;
    int count = 0;
    while (av[i])
    {
        if (av[i] >= '0' && av[i] <= '9')
            count++;
        i++;
    }
    return count;
}

int count_uppercase(const char *av)
{
    int i = 0;
    int count = 0;
    while (av[i])
    {
        if (av[i] >= 'A' && av[i] <= 'Z')
            count++;
        i++;
    }
    return count;
}

int count_lowercase(const char *av)
{
    int i = 0;
    int count = 0;
    while (av[i])
    {
        if (av[i] >= 'a' && av[i] <= 'z')
            count++;
        i++;
    }
    return count;
}

int count_special(const char *av)
{
    int i = 0;
    int count = 0;
    while (av[i])
    {
        if (!std::isdigit(av[i]) && !std::isalpha(av[i]))
            count++;
        i++;
    }
    return count;
}
