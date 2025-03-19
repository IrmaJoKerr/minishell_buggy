/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 17:45:41 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:32:41 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_substr allocates (with malloc(3)) and returns a substring from the
string ’s’. The substring begins at index ’start’ and is of maximum size ’len’.
*/

#include "libft.h"

static char	*ft_emptystr(void)
{
	char	*empty_string;

	empty_string = malloc(1);
	if (empty_string != NULL)
		empty_string[0] = '\0';
	return (empty_string);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	s_len;
	size_t	sub_len;
	size_t	i;

	if (!s)
		return (NULL);
	s_len = ft_strlen(s);
	i = 0;
	if (start >= s_len)
		return (ft_emptystr());
	sub_len = s_len - start;
	if (sub_len > len)
		sub_len = len;
	substr = malloc(sub_len + 1);
	if (substr == NULL)
		return (NULL);
	while (i < sub_len)
	{
		substr[i] = s[start + i];
		i++;
	}
	substr[sub_len] = '\0';
	return (substr);
}
