/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdelimiter.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 09:05:39 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 09:39:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_isdelimiter() function tests whether c is a delimiter character.
char * delimiters is a string of delimiter characters to be tested against c.
The function returns a 1 if c is a delimiter character.
Else it returns a 0.
*/
#include "libft.h"

int	ft_isdelimiter(char c, char *delimiters)
{
	while (*delimiters)
	{
		if (c == *delimiters)
			return (1);
		delimiters++;
	}
	return (0);
}
