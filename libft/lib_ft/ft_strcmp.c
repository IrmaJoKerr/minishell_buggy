/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/27 09:09:29 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 03:18:23 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_strcmp() function compares the two strings s1 and s2. The comparison
is done using unsigned characters. The function returns an integer indicating
the result of the comparison, as follows:
•  0, if the s1 and s2 are equal;
•  a negative value if s1 is less than s2;
•  a positive value if s1 is greater than s2.
*/

#include "libft.h"

int	ft_strcmp(const char *str1, const char *str2)
{
	while (*str1 && *str2)
	{
		if (*str1 != *str2)
			return ((unsigned char)*str1 - (unsigned char)*str2);
		str1++;
		str2++;
	}
	return ((unsigned char)*str1 - (unsigned char)*str2);
}
