/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 17:48:39 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 04:04:48 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_strtrim allocates (with malloc(3)) and returns a copy of ’s1’ with the 
characters specified in ’set’ removed from the beginning and the end of the
string.
*/

#include "libft.h"

static int	chk_set(char c, const char *set)
{
	while (*set)
	{
		if (c == *set)
			return (1);
		set++;
	}
	return (0);
}

static char	*ft_copy(const char *s1, size_t start, size_t end)
{
	size_t	i;
	char	*newstr;

	i = 0;
	newstr = (char *)malloc(end - start + 1);
	if (!newstr)
		return (NULL);
	while (i < end - start)
	{
		newstr[i] = s1[start + i];
		i++;
	}
	newstr[end - start] = '\0';
	return (newstr);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	start;
	size_t	end;
	size_t	len;

	if (!s1 || !set)
		return (NULL);
	start = 0;
	end = 0;
	len = 0;
	while (s1[len] != '\0')
		len++;
	end = len;
	while (start < end && chk_set(s1[start], set))
		start++;
	while (end > start && chk_set(s1[end - 1], set))
		end--;
	return (ft_copy(s1, start, end));
}
