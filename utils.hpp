#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdlib.h>

char	**ft_split(char const *s, char c);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strjoin_irc(char const *s1, char const *s2);
char	**ft_split_pattern(char const *s, const char *patt);
#endif