/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 06:38:38 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:42:56 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strnstr() function locates the first occurrence of the null-terminated
string "find" in the string "pile", where not more than len characters are
searched. Characters that appear after a ‘\0’ character are not searched.
If "find" is an empty string, "pile" is returned; if "find" occurs nowhere
in "pile", NULL is returned; otherwise a pointer to the first character of
the first occurrence of "find" is returned.
*/

#include "libft.h"

char	*ft_strnstr(const char *pile, const char *find, size_t len)
{
	size_t	findlen;
	size_t	len1;

	len1 = len;
	if (!*find)
		return ((char *)pile);
	findlen = ft_strlen(find);
	if (!pile && len1 == 0)
		return (NULL);
	while (*pile && len-- >= findlen)
	{
		if (*pile == *find && !ft_memcmp(pile, find, findlen))
			return ((char *)pile);
		pile++;
	}
	return (NULL);
}
