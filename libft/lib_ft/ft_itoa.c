/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 17:49:37 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:29:57 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_itoa allocates (with malloc(3)) and returns a string representing the
integer received as an argument. Negative numbers are handled.
*/

#include "libft.h"

static char	*ft_strncpy(char *dest, char *src, size_t n)
{
	char	*oridest;

	oridest = dest;
	while (n-- && *src != '\0')
	{
		*dest = *src;
		dest++;
		src++;
	}
	while (n--)
	{
		*dest = '\0';
		dest++;
	}
	return (oridest);
}

static int	num_len(int n)
{
	int	len;

	len = 0;
	if (n <= 0)
		len++;
	while (n != 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

static void	ft_convert(char *str, int n, int len)
{
	while (n >= 10)
	{
		len--;
		str[len] = (n % 10) + '0';
		n /= 10;
	}
	str[len - 1] = n + '0';
}

char	*ft_itoa(int n)
{
	char	*str;
	int		len;

	len = num_len(n);
	str = malloc((len + 1) * sizeof(char));
	if (!str)
		return (NULL);
	str[len] = '\0';
	if (n < 0)
	{
		str[0] = '-';
		n = -n;
		if (n == -2147483648)
			return (ft_strncpy(str, "-2147483648", 12));
	}
	ft_convert(str, n, len);
	return (str);
}
