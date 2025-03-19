/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoiarr_errcln.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 23:24:19 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 03:07:44 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_atoiarr_errcln is a function that converts a 2D array of strings into
an array of integers. Note that it will FREE the 2D array of strings after
conversion.
ONLY USE THIS FUNCTION IF YOU NO LONGER NEED THE 2D ARRAY OF STRINGS. 
OR IF THE 2D ARRAY OF STRINGS IS MALLOCCED AND YOU WANT TO FREE IT AFTER.
Designed to work with ft_atoierr() to handle errors.
*/

#include "libft.h"

static void	ft_error(char **temparr, int *intarr, int len);
static int	*convert_to_intarr(char **temparr, int len);
static void	ft_error(char **temparr, int *intarr, int len);

static void	ft_error(char **temparr, int *intarr, int len)
{
	if (temparr != NULL)
		ft_free_2d(temparr, len);
	if (intarr != NULL)
		ft_safefree((void **)&intarr);
}

static int	*convert_to_intarr(char **temparr, int len)
{
	int			*intarr;
	int			i;
	long long	temp;

	intarr = malloc(len * sizeof(int));
	if (!intarr)
		return (NULL);
	i = 0;
	while (i < len)
	{
		temp = ft_atoierr(temparr[i]);
		if (temp > INT_MAX || temp < INT_MIN)
		{
			ft_safefree((void **)&intarr);
			write(2, "Error: Integer out of INT range\n", 32);
			return (NULL);
		}
		intarr[i] = (int)temp;
		i++;
	}
	return (intarr);
}

int	*ft_atoiarr_errcln(char **temparr, int len)
{
	int	*intarr;

	len = ft_arrlen(temparr);
	intarr = convert_to_intarr(temparr, len);
	if (!intarr)
	{
		ft_error(temparr, NULL, len);
		return (NULL);
	}
	ft_free_2d(temparr, len);
	return (intarr);
}
