/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:08 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:12:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strchr() function returns a pointer to the first occurrence of the
character "character" in the string str. The function returns a pointer to the
matched character or NULL if the character is not found. The terminating null
byte is considered part of the string, so that if c is specified as '\0', the
function returns a pointer to the terminator.
*/

#include "libft.h"

char	*ft_strchr(const char *str, int character)
{
	unsigned char	c;

	c = (unsigned char)character;
	while (*str != '\0')
	{
		if (*str == c)
			return ((char *)str);
		str++;
	}
	if (c == '\0')
		return ((char *)str);
	return (NULL);
}
