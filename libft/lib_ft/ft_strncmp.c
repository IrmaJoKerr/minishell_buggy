/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:40 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:46:32 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strncmp() function compares not more than n bytes (bytes that follow
a NUL character are not compared) from the array pointed to by str1 to the
array pointed to by str2.The sign of a non-zero return value is determined
by the sign of the difference between the values of the first pair of bytes
(both interpreted as type unsigned char) that differ in the strings being
compared. Upon successful completion, the function returns an integer
greater than, equal to, or less than 0, if the possibly null-terminated
array pointed to by str1 is greater than, equal to, or less than the possibly
null-terminated array pointed to by str2 respectively.
*/

#include "libft.h"

int	ft_strncmp(const char *str1, const char *str2, size_t n)
{
	while (n > 0 && *str1 && *str2)
	{
		if (*str1 != *str2)
			return ((unsigned char)*str1 - (unsigned char)*str2);
		str1++;
		str2++;
		n--;
	}
	if (n > 0)
	{
		if (*str1 == '\0' && *str2 != '\0')
			return (-1);
		else if (*str1 != '\0' && *str2 == '\0')
			return (1);
		return (0);
	}
	return (0);
}
