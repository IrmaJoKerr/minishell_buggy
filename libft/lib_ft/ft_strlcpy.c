/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:02:43 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:56:32 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strlcpy() function copies strings. It is designed to be a safer, more
consistent, and less error prone replacement for strncpy(3).Unlike strncpy(3),
ft_strlcpy() takes the full size of the buffer (not just the length) and
guarantee to NUL-terminate the result (as long as size is larger than 0 and
the buffer has at least one byte of space). A byte for the NUL is included
in size.  Also note that ft_strlcpy() operates only on true “C” strings.
This means that for ft_strlcpy() src must be NUL-terminated. The ft_strlcpy()
function copies up to size - 1 characters from the NUL-terminated string src
to dest, NUL-terminating the result. The function returns the total length of
the string they tried to create(the length of src).
*/

#include "libft.h"

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	max_len;
	size_t	src_len;

	i = 0;
	src_len = 0;
	if (size > 0)
		max_len = size - 1;
	else
		max_len = 0;
	while (i < max_len && src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	if (size > 0)
		dest[i] = '\0';
	while (src[src_len] != '\0')
		src_len++;
	return (src_len);
}
