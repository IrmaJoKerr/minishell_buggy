/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_x_upper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/21 22:00:03 by bleow             #+#    #+#             */
/*   Updated: 2024/09/24 21:11:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/ft_printf.h"

static int	ft_convert_hexup(unsigned int num)
{
	int		count;
	char	printnum;

	count = 0;
	if (num == 0)
	{
		write(1, "0", 1);
		return (1);
	}
	if (num >= 16)
		count += ft_convert_hexup(num / 16);
	printnum = (num % 16);
	if (printnum < 10)
		printnum += '0';
	else
		printnum += 'A' - 10;
	write(1, &printnum, 1);
	count++;
	return (count);
}

void	ft_arg_x_upper(unsigned int hexup, unsigned int *printed)
{
	*printed += ft_convert_hexup(hexup);
}
