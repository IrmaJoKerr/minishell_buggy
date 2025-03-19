/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:02:19 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:17:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_memmove() function copies n bytes from memory area s to memory area d.
The memory areas may overlap: copying takes place as though the bytes in s are
first copied into a temporary array that does not overlap s or d, and the
bytes are then copied from the temporary array to d.
*/

#include "libft.h"

static void	ft_con_a(unsigned char *d, const unsigned char *s, size_t n)
{
	while (n > 0)
	{
		d[n - 1] = s[n - 1];
		n--;
	}
}

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;

	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	if (!d && !s)
		return (NULL);
	if (s < d && d < s + n)
	{
		ft_con_a(d, s, n);
	}
	else
	{
		while (n--)
		{
			*d = *s;
			d++;
			s++;
		}
	}
	return (dest);
}
