#include "ft_irc.hpp"

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
    std::stringstream ss(argv[1]);
    int nb;
    ss >> nb;
    std::cout << "port = " << nb << ", password = " << argv[2] << std::endl;
    return 0;
}
