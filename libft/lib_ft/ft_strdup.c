/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 05:11:04 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:12:26 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strdup() function returns a pointer to a new string which is a
duplicate of the string str. Memory for the new string is obtained with
malloc(3), and can be freed with free(3). On success, the ft_strdup() function
returns a pointer to the duplicated string. It returns NULL if insufficient
memory was available.
*/

#include "libft.h"

char	*ft_strdup(const char *str)
{
	char	*dup;
	size_t	len;

	len = ft_strlen(str);
	dup = malloc(len + 1);
	if (!dup || !str)
		return (NULL);
	ft_memcpy(dup, str, len);
	dup[len] = '\0';
	return (dup);
}
