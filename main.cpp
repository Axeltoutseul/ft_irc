#include "ft_irc.hpp"

void check_port(const char *arg)
{
    if (!is_digit(arg))
    {
        std::cerr << "Error: The ID must have only digits" << std::endl;
        exit(1);
    }
    std::stringstream ss(arg);
    int port;
    ss >> port;
    create_server(port);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Error: You must have two arguments: an ID and a password, for example : 4040 new_server" << std::endl;
        return -1;
    }
    check_port(argv[1]);
    std::string nickname;
    std::cout << "What's your nickname ? ";
    std::getline(std::cin, nickname);
    while (!valid_nickname(nickname))
    {
        std::cout << "You must enter a valid name." << std::endl;
        std::cout << "What's your nickname ? ";
        std::getline(std::cin, nickname);
    }
    std::cout << "Your name is " << nickname << std::endl;
    return 0;
}
