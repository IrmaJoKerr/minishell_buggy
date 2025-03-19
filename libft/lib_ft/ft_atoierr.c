/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoierr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 23:07:53 by bleow             #+#    #+#             */
/*   Updated: 2024/11/09 04:03:24 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_atoierr() function converts the initial portion of the string pointed
to by nptr to int. The return is the long long converted value LLONG_MAX or
LLONG_MIN to indicate error. It is so another function can handle the error
accordingly, such as freeing memory and exiting the program.
*/

#include "libft.h"

static long long	calc(long long res, int sign, const char *str, int i)
{
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (res > (LLONG_MAX - (str[i] - '0')) / 10)
		{
			if (sign == 1)
				return (LLONG_MAX);
			else
				return (LLONG_MIN);
		}
		res = res * 10 + (str[i++] - '0');
	}
	return (res);
}

long long	ft_atoierr(const char *str)
{
	long long	res;
	int			sign;
	int			i;

	res = 0;
	sign = 1;
	i = 0;
	if (!str)
		return (0);
	while ((str[i] >= 9 && str[i] <= 13) || (str[i] == 32))
	{
		i++;
		if (i == (int)ft_strlen(str))
			return (0);
	}
	if (str[i] == '-' || str[i] == '+')
		sign = 1 - 2 * (str[i++] == '-');
	res = calc(res, sign, str, i);
	if ((res == LLONG_MAX) || (res == LLONG_MIN))
		return (res);
	return ((sign * res));
}
