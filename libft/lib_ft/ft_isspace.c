/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isspace.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:55:07 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 12:18:16 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_isspace() function tests for white-space characters.
Returns 1 if c is a whitespace character; otherwise, it returns 0.
Whitespace characters are: space, tab, newline, carriage return,
vertical tab, and form feed.
*/
#include "libft.h"

int	ft_isspace(int c)
{
	if (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r')
		return (1);
	else
		return (0);
}
