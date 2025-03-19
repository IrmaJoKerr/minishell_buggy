/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/12 14:16:17 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 03:04:36 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_split allocates (with malloc(3)) and returns an array of strings obtained
by splitting ’s’ using the character ’c’ as a delimiter. The array ends with
a NULL pointer.
*/

#include "libft.h"

static size_t	count_substr(char const *s, char delimiter)
{
	size_t	count;

	count = 0;
	while (*s != '\0')
	{
		while (*s == delimiter)
			s++;
		if (*s != '\0')
			count++;
		while (*s && *s != delimiter)
			s++;
	}
	return (count);
}

static void	free_splits(char **splits, size_t i)
{
	while (i > 0)
	{
		i--;
		ft_safefree((void **)&splits[i]);
	}
	ft_safefree((void **)&splits);
}

static char	*find_next_substring(char **start, char delimiter)
{
	char	*end;
	char	*substring;
	size_t	len;
	size_t	i;

	while (**start == delimiter)
		(*start)++;
	if (**start == '\0')
		return (NULL);
	end = *start;
	while (*end && *end != delimiter)
		end++;
	len = end - *start;
	substring = malloc(len + 1);
	if (!substring)
		return (NULL);
	i = 0;
	while (i < len)
	{
		substring[i] = (*start)[i];
		i++;
	}
	substring[len] = '\0';
	*start = end;
	return (substring);
}

char	**ft_split(char const *s, char delimiter)
{
	char	**splits;
	size_t	substr_count;
	size_t	i;
	char	*start;

	if (!s)
		return (NULL);
	substr_count = count_substr(s, delimiter);
	splits = malloc((substr_count + 1) * sizeof(char *));
	if (!splits)
		return (NULL);
	i = 0;
	start = (char *)s;
	while (i < substr_count)
	{
		splits[i] = find_next_substring(&start, delimiter);
		if (!splits[i])
		{
			free_splits(splits, i);
			return (NULL);
		}
		i++;
	}
	splits[i] = NULL;
	return (splits);
}
