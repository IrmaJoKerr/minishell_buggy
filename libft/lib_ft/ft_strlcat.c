/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:03:08 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:05:45 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strlcat() function concatenates strings. It is designed to be safer,
more consistent, and less error prone replacement for strncat(3).  Unlike
strncat(3), ft_strlcat() takes the full size of the buffer (not just the
length) and guarantees to NUL-terminate the result (as long as size is
larger than 0 and as long as there is at least one byte free in dest).A byte
for the NUL is included in size.  Also note that ft_strlcat() only operates on
true “C” strings(both src and dst must be NUL-terminated).
The ft_strlcat() function appends the NUL-terminated string src to the end of
dest.It will append at most size - ft_strlen(dest) - 1 bytes, NUL-terminating
the result. The function returns the total length of the string it tried to
create. That means the initial length of dest plus the length of src. While
this may seem somewhat confusing, it was done to make truncation detection
simple.
Note, however, that if ft_strlcat() traverses size characters without finding
a NUL, the length of the string is considered to be size and the dest string
will not be NUL-terminated (since there was no space for the NUL). This keeps
strlcat() from running off the end of a string.  In practice this should not
happen (as it means that either size is incorrect or that dest is not a proper
“C” string). The check exists to prevent potential security problems in
incorrect code.
*/

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	dest_len;
	size_t	src_len;
	size_t	i;

	if (!dest && size == 0)
		return (0);
	dest_len = ft_strlen(dest);
	src_len = ft_strlen(src);
	i = 0;
	if (size <= dest_len)
		return (src_len + size);
	while (src[i] != '\0' && dest_len + i < size - 1)
	{
		dest[dest_len + i] = src[i];
		i++;
	}
	if (dest_len + i < size)
		dest[dest_len + i] = '\0';
	return (dest_len + src_len);
}
