#include "utils.hpp"
#include <stdio.h>

size_t ft_strlen(const char *str)
{
    size_t i = 0;

    while(str[i])
        i++;
    return(i);
}


char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char				*str;
	unsigned int		i;

	i = 0;
	if (!s)
		return (NULL);
	if (start > ft_strlen(s))
	{
		str = (char *)malloc(sizeof(char) * 1);
		if (!str)
			return (NULL);
		str[0] = 0;
		return (str);
	}
	else if (len > ft_strlen(s) - start)
		str = (char *)malloc(sizeof(char) * (ft_strlen(s) - start + 1));
	else
		str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	while (i < len && s[start])
		str[i++] = s[start++];
	str[i] = 0;
	return (str);
}


int	strnbr(char const *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] && s[i] != c)
		{
			while (s[i] && s[i] != c)
				i++;
			count++;
		}
		while (s[i] && s[i] == c)
			i++;
	}
	return (count);
}

char	**ft_split(char const *s, char c)
{
	char	**str;
	int		i;
	int		j;
	int		k;

	i = 0;
	j = 0;
	k = 0;
	str = (char **)malloc(sizeof(char *) * (strnbr(s, c) + 1));
	if (!str)
		return (NULL);
	while (i < strnbr(s, c))
	{
		while (s[j] == c && s[j])
			j++;
		while (s[j] != c && s[j])
		{
			k++;
			j++;
		}
		str[i++] = ft_substr(s, (j - k), k);
		k = 0;
	}
	str[i] = NULL;
	return (str);
}

char	*ft_strjoin_irc(char const *s1, char const *s2)
{
	char		*str;
	size_t		i;
	size_t		j;

	str = (char *)malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 2));
	if (!str)
		return (NULL);
	i = 0;
	j = 0;
	if (s1) {
		while (i < ft_strlen(s1))
		{
			str[i] = s1[i];
			i++;
		}
	}
	if (i != 0)
		str[i] = ' ';
	while (j < ft_strlen(s2))
	{
		str[i] = s2[j];
		i++;
		j++;
	}
	str[i] = 0;
	return (str);
}


int	ft_strncmp(const char *string1, const char *string2, size_t count)
{
	unsigned int	i;

	i = 0;
	if (count == 0)
		return (0);
	while ((string1[i] == string2[i]) && (i < count - 1) && (string1[i] != 0))
		i++;
	return (((unsigned char)string1[i]) - ((unsigned char)string2[i]));
}

int	strnbr_pattern(char const *s, const char *patt)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] && s[i] != patt[0])
		{
			while (s[i] && s[i] != patt[0])
				i++;
			count++;
		}
		int j = 0;
		while (s[i + j] && s[i + j] == patt[j] && patt[j]) {
			j++;
			printf("%d\n", j);
		}
		if ((size_t)j == ft_strlen(patt)) {
			i = i + j;
		}
	}
	return (count);
}

char	**ft_split_pattern(char const *s, const char *patt)
{
	char	**str;
	int		i;
	int		j;
	int		k;

	i = 0;
	j = 0;
	k = 0;


	// printf("%s\n", s);
	printf("strnbr_pattern > %d\n", strnbr_pattern(s, patt));
	str = (char **)malloc(sizeof(char *) * (strnbr_pattern(s, patt) + 1));
	if (!str)
		return (NULL);
	while (i < strnbr_pattern(s, patt))
	{
		if (!ft_strncmp(s + i, patt, ft_strlen(patt))){
			j = j + ft_strlen(patt);
			printf("here\n");
		}
		while (s[j] != patt[0] && s[j])
		{
			k++;
			j++;
		}
		str[i++] = ft_substr(s, (j - k), k);
		k = 0;
	}
	str[i] = NULL;
	return (str);
}