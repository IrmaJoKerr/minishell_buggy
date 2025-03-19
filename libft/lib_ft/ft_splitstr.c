/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_splitstr.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 00:23:51 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 09:58:55 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_splitstr is an enhanced version of ft_split.
ft_splitstr allocates (with malloc(3)) and returns an array of strings obtained
by splitting ’s’ using the string of characters ’delimiters’ as a delimiter.
ft_splitstr is able to handle quoted strings, where delimiters are inside quotes.
It will treat the quoted strings as a single string.
The function loops through the delimiter string, checking if any of the
characters in the string matches the character in the string ’s’. If a match
occurs for any of the characters in the delimiter string, the function will
consider the character in ’s’ as a delimiter.The array ends with a 
NULL pointer.
*/

#include "libft.h"

static size_t	count_substr(char const *s, char *delimiters)
{
	size_t	count;
	int		in_substr;
	int		in_quotes;
	char	quote_char;

	count = 0;
	in_substr = 0;
	in_quotes = 0;
	quote_char = '\0';
	while (*s != '\0')
	{
		if (ft_isdelimiter_quote(*s, delimiters, &in_quotes, &quote_char))
			in_substr = 0;
		else if (in_substr == 0)
		{
			in_substr = 1;
			count++;
		}
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

static char	*find_next_substring(char **start, char *delimiters)
{
	char	*end;
	char	*substring;
	int		in_quo;
	char	quo_char;
	size_t	len;

	in_quo = 0;
	quo_char = '\0';
	while (**start && ft_isdelimiter_quote(**start, delimiters, &in_quo,
			&quo_char))
		(*start)++;
	if (!**start)
		return (NULL);
	end = *start;
	in_quo = 0;
	quo_char = '\0';
	while (*end && !ft_isdelimiter_quote(*end, delimiters, &in_quo, &quo_char))
		end++;
	len = end - *start;
	substring = malloc(len + 1);
	if (!substring)
		return (NULL);
	ft_strlcpy(substring, *start, end - *start + 1);
	*start = end;
	return (substring);
}

char	**ft_splitstr(char const *s, char *delimiters)
{
	char	**splits;
	char	*start;
	size_t	substr_count;
	size_t	i;

	if (!s || !delimiters)
		return (NULL);
	substr_count = count_substr(s, delimiters);
	splits = malloc((substr_count + 1) * sizeof(char *));
	if (!splits)
		return (NULL);
	i = 0;
	start = (char *)s;
	while (i < substr_count)
	{
		splits[i] = find_next_substring(&start, delimiters);
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
