/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_u.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/20 19:50:26 by bleow             #+#    #+#             */
/*   Updated: 2024/09/24 21:11:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/ft_printf.h"

static int	ft_convert(unsigned int num)
{
	int		count;
	char	printnum;

	count = 0;
	if (num == 0)
	{
		write(1, "0", 1);
		return (count + 1);
	}
	else if (num >= 10)
		count += ft_convert(num / 10);
	printnum = '0' + (num % 10);
	write(1, &printnum, 1);
	count++;
	return (count);
}

void	ft_arg_u(unsigned int num, unsigned int *printed)
{
	*printed += ft_convert(num);
}
