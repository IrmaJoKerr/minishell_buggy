/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:23 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:39:24 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strrchr() function locates the last occurrence of c (converted
to a char) in the string pointed to by s. The terminating NUL character
is considered to be part of the string. Upon successful completion,
the function returns a pointer to the byte or a null pointer if c does
not occur in the string.
*/

#include "libft.h"

char	*ft_strrchr(const char *str, int character)
{
	const char		*last_occurrence;
	unsigned char	c;

	c = (unsigned char)character;
	last_occurrence = NULL;
	while (*str != '\0')
	{
		if (*str == c)
			last_occurrence = str;
		str++;
	}
	if (c == '\0')
		return ((char *)str);
	return ((char *)last_occurrence);
}
