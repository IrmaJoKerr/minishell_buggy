/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 17:54:16 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:21:19 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_isalpha() checks for an alphabetic character; in the standard "C". The
values returned are nonzero if the character c falls into the tested
class, and zero if not.
*/

#include "libft.h"

int	ft_isalpha(int c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	else
		return (0);
}
