/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:06:06 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 10:35:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_atoi() function converts the initial portion of the string pointed to by
nptr to int. The return is the converted value or 0 on error.
*/

#include "libft.h"

static int	calc(long long res, int sign, const char *str, int i)
{
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (res > (LLONG_MAX - (str[i] - '0')) / 10)
		{
			if (sign == 1)
				return (INT_MAX);
			else
				return (INT_MIN);
		}
		res = res * 10 + (str[i++] - '0');
	}
	return (res);
}

int	ft_atoi(const char *str)
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
	return ((int)(sign * res));
}
