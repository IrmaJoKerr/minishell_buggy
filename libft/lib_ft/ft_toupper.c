/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:03:24 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:36:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_toupper converts lowercase letters to uppercase. If c is a lowercase
letter, the function returns its uppercase equivalent, Otherwise, it returns c.
*/

#include "libft.h"

int	ft_toupper(int c)
{
	if (c >= 97 && c <= 122)
		return (c - 32);
	else
		return (c);
}
