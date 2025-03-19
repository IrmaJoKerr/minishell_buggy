/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 17:55:27 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:12:56 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_isascii() function tests whether c is a 7-bit US-ASCII character code.
The function returns a non-zero if c is a 7-bit US-ASCII character code
between 0 and octal 0177 inclusive; otherwise, it returns 0.
*/

#include "libft.h"

int	ft_isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);
	else
		return (0);
}
