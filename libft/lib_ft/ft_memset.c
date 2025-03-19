/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 17:56:41 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:15:02 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_memset() function fills the first count bytes of the memory area
pointed to by dest with the constant byte c. The function returns a pointer
to the memory area dest.
*/

#include "libft.h"

void	*ft_memset(void *dest, int c, size_t count)
{
	unsigned char	*ptr;

	ptr = (unsigned char *)dest;
	while (count > 0)
	{
		*ptr = (unsigned char)c;
		++ptr;
		--count;
	}
	return (dest);
}
