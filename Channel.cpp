#include "Channel.hpp"

Channel::Channel() {}
Channel::~Channel() {}

void Channel::execCommand(const std::string command)
{
    std::vector<std::string> elements;
    std::istringstream iss(command);
    std::string line;

    while (iss >> line)
        elements.push_back(line);
    if (elements.size() == 0 || elements.size() > 2)
        return;

    if (elements[0] == "INVITE")
        std::cout << "Invite command" << std::endl;
    else if (elements[0] == "KICK")
        std::cout << "Kick command" << std::endl;
    else if (elements[0] == "TOPIC")
        std::cout << "Topic command" << std::endl;
    else if (elements[0] == "MODE")
    {
        if (elements.size() < 2)
            return;
        if (elements[1] == "-i")
            std::cout << "Invite only channel" << std::endl;
        else if (elements[1] == "-t")
            std::cout << "Restrictions of the Topic" << std::endl;
        else if (elements[1] == "-k")
            std::cout << "Channel key" << std::endl;
        else if (elements[1] == "-k")
            std::cout << "Channel operator privilege" << std::endl;
        else if (elements[1] == "-u")
            std::cout << "User limit to channel" << std::endl;
        else
            return;
    }
    else
        return;
}