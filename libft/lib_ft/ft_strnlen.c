/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnlen.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 17:19:18 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 03:28:33 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strnlen() function returns the number of bytes in the string pointed to
by s, excluding the terminating null byte ('\0'), but at most maxlen. In doing
this, ft_strnlen() looks only at the first maxlen characters in the string 
pointed to by s and never beyond s[maxlen-1]. The strnlen() function returns
strlen(s), if that is less than maxlen, or maxlen if there is no null 
terminating ('\0') among the first maxlen characters pointed to by s.
*/

#include "libft.h"

size_t	ft_strnlen(const char *s, size_t maxlen)
{
	size_t	len;

	len = 0;
	while (len < maxlen && s[len] != '\0')
	{
		len++;
	}
	return (len);
}
