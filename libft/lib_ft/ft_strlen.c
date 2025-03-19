/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 17:56:17 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:47:32 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strlen() function calculates the length of the string pointed to by s,
excluding the terminating null byte ('\0'). The strlen() function returns the
number of bytes in the string pointed to by s.
*/

#include "libft.h"

size_t	ft_strlen(const char *str)
{
	const char	*s;

	s = str;
	while (*s)
		s++;
	return (s - str);
}
