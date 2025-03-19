/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoiarr_cln.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 02:29:06 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 03:08:25 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_atoiarr_cln is a function that converts a 2D array of strings into an array of
integers. Note that it will FREE the 2D array of strings after conversion.
ONLY USE THIS FUNCTION IF YOU NO LONGER NEED THE 2D ARRAY OF STRINGS. 
OR IF THE 2D ARRAY OF STRINGS IS MALLOCCED AND YOU WANT TO FREE IT AFTER.
*/

#include "libft.h"

int	*ft_atoiarr_cln(char **temparr, int len)
{
	int	*intarr;
	int	i;

	len = ft_arrlen(temparr);
	intarr = malloc(len * sizeof(int));
	if (!intarr)
	{
		write (2, "Error\n", 6);
		exit (1);
	}
	i = 0;
	while (i < len)
	{
		intarr[i] = ft_atoi(temparr[i]);
		i++;
	}
	i = 0;
	while (i < len)
	{
		ft_safefree((void **)&temparr[i]);
		i++;
	}
	ft_safefree((void **)&temparr);
	return (intarr);
}
