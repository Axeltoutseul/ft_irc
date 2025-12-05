#include "ft_irc.hpp"

t_env *init_env(char **argv)
{
    t_env *env = new t_env();
    env->port = std::atoi(argv[1]);
    return env;
}

void create_server(int port)
{
    //struct protoent *pe = getprotobyname("tcp");
    //int server = socket(PF_INET, SOCK_STREAM, pe->p_proto);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);
}
