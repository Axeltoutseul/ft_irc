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

#endif
