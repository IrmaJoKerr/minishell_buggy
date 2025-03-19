/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 10:27:56 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 02:59:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*ft_gstrjoin(char *s1, char *s2)
{
	size_t	len1;
	size_t	len2;
	char	*s3;

	if (!s1)
	{
		s1 = (char *)malloc(1 * sizeof(char));
		if (!s1)
			return (NULL);
		s1[0] = '\0';
	}
	if (!s2)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	s3 = malloc((len1 + len2 + 1) * sizeof(char));
	if (!s3)
		return (NULL);
	ft_memcpy(s3, s1, len1);
	ft_memcpy(s3 + len1, s2, len2);
	s3[len1 + len2] = '\0';
	ft_safefree((void **)&s1);
	return (s3);
}
