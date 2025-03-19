/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:02:03 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:20:23 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_memcpy() function copies n bytes from memory area src to memory area
dest. The function DOES NOT CHECK if memory areas overlap. The function returns
a pointer to dest.
*/

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t		i;
	char		*p_dest;
	const char	*p_src;

	p_dest = (char *)dest;
	p_src = (const char *)src;
	i = 0;
	if (!p_dest && !p_src)
		return (NULL);
	while (i < n)
	{
		p_dest[i] = p_src[i];
		i++;
	}
	return (dest);
}
