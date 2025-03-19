/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:05:36 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:22:55 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_memcmp() function compares the first n bytes (each interpreted as
unsigned char) of the memory areas s1 and s2.
The function returns an integer less than, equal to, or greater than zero
if the first n bytes of s1 is found, respectively, to be less than, to match,
or be greater than the first n bytes of s2.
For a nonzero return value, the sign is determined by the sign of the
difference between the first pair of bytes (interpreted as unsigned char)
that differ in s1 and s2. If n is zero, the return value is zero.
*/

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*p1;
	const unsigned char	*p2;

	p1 = s1;
	p2 = s2;
	while (n--)
	{
		if (*p1 != *p2)
		{
			return (*p1 - *p2);
		}
		p1++;
		p2++;
	}
	return (0);
}
