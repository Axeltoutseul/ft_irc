#ifndef FT_IRC_HPP
#define FT_IRC_HPP
#include <iostream>
#include <bits/stdc++.h>
#include <termios.h>
#include <sys/socket.h>
#include "bircd/bircd.h"



int is_digit(const char *av);
int count_digits(const char *av);
int count_uppercase(const char *av);
int count_lowercase(const char *av);
int count_special(const char *av);

#endif
