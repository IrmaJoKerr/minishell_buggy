/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:05:08 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:25:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_memchr() function scans the initial n bytes of the memory area pointed
to by str for the first instance of c. Both c and the bytes of the memory
area pointed to by str are interpreted as unsigned char. The function returns
a pointer to the matching byte or NULL if the character does not occur in the
given memory area.
*/

#include "libft.h"

void	*ft_memchr(const void *str, int c, size_t n)
{
	unsigned char		target;
	const unsigned char	*src;

	src = (const unsigned char *)str;
	target = (unsigned char)c;
	while (n--)
	{
		if (*src == target)
		{
			return ((void *)(src));
		}
		src++;
	}
	return (NULL);
}
