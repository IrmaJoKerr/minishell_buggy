/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 03:39:49 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 04:07:14 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_calloc() function allocates unused space for an array of num elements
each of whose size in bytes is size.  The space is initialized to all bits 0.
Upon successful completion with both num and size non-zero, calloc()
returns a pointer to the allocated space. If either num or size is 0,
then a pointer to an allocated space of 1 shall be returned.
*/

#include "libft.h"

void	*ft_calloc(size_t num, size_t size)
{
	size_t	total_size;
	void	*alloc;

	alloc = 0;
	if (num == 0 || size == 0)
	{
		alloc = malloc(1);
		if (!alloc)
			return (NULL);
		if (alloc)
			ft_bzero(alloc, 1);
		return (alloc);
	}
	total_size = num * size;
	if (total_size > SIZE_MAX || total_size > INT_MAX)
		return (NULL);
	if (num > 0 && size > 0)
		alloc = malloc(total_size);
	if (!alloc)
		return (NULL);
	ft_bzero(alloc, total_size);
	return (alloc);
}
