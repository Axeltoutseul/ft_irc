#ifndef FT_IRC_HPP
#define FT_IRC_HPP
#include <iostream>
#include <bits/stdc++.h>
#include <termios.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

struct t_env
{
    int *fds;
    int max_fd;
    int port;
};

t_env *init_env(char **argv);
void create_server(int port);
int is_digit(const char *av);
int count_digits(const char *av);
int count_uppercase(const char *av);
int count_lowercase(const char *av);
int count_special(const char *av);

#endif
