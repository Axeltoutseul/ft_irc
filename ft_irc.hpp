#ifndef FT_IRC_HPP
#define FT_IRC_HPP
#include <iostream>
#include <bits/stdc++.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <regex.h>
#include <ctype.h>

void create_server(int port);
int is_digit(const char *av);
int is_in_charset(int carac, const std::string charset);
int valid_nickname(const std::string nickname);

#endif
