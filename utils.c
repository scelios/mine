#include "nm.h"

int	ft_strncmp(const char *first, const char *second, size_t length)
{
	size_t	i;

	i = 0;
	while (i < length)
	{
		if (((unsigned char)first[i] != (unsigned char)second[i]
				|| first[i] == 0) || second[i] == 0)
			return ((unsigned char)first[i] - (unsigned char)second[i]);
		i++;
	}
	return (0);
}

void	ft_putstr_fd(int fd, char *s)
{
	int	i;

	if (!s)
		return ;
	i = 0;
	while (s[i])
	{
		write(fd, &s[i], 1);
		i++;
	}
}

size_t	ft_strlen(const char *theString)
{
	int	i;

	i = 0;
	while (theString[i])
		i++;
	return (i);
}

bool str_is_nullterm(const char *start, const char *end)
{
	while (start < end)
	{
		if (!*start)
			return true;
		++start;
	}

	return false;
}
