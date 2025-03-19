/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:03:42 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:37:11 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_tolower converts uppercase letters to lowercase. If c is a uppercase
letter, the function returns its lowercase equivalent, Otherwise, it returns c.
*/

#include "libft.h"

int	ft_tolower(int c)
{
	if (c >= 65 && c <= 90)
		return (c + 32);
	else
		return (c);
}
