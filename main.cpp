#include "ft_irc.hpp"


int valid_password(const char *av)
{
    if (std::strlen(av) < 8)
        std::cout << "Your password is too short" << std::endl;
    else if (is_digit(av))
    {
        std::cout << "Your password contains only digits";
        return 0;
    }
    else if (!count_uppercase(av) || !count_lowercase(av) || !count_digits(av) || !count_special(av))
    {
        std::cout << "Your password must contain at least 1 lowercase, 1 uppercase, 1 digit and 1 special character." << std::endl;
        return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Error: You must have two arguments: an ID and a password, for example : 4040 new_server" << std::endl;
        return -1;
    }
    if (!is_digit(argv[1]))
    {
        std::cerr << "Error: The ID must have only digits" << std::endl;
        return -1;
    }
    return 0;
}
