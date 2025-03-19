/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_p.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/20 19:50:04 by bleow             #+#    #+#             */
/*   Updated: 2024/09/24 21:11:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/ft_printf.h"

static int	ft_convert_hexptr(uintptr_t num)
{
	int		count;
	char	printnum;

	count = 0;
	if (num >= 16)
		count += ft_convert_hexptr(num / 16);
	printnum = (num % 16);
	if (printnum < 10)
		printnum += '0';
	else
		printnum += 'a' - 10;
	write(1, &printnum, 1);
	count++;
	return (count);
}

void	ft_arg_p(void *ptr, unsigned int *printed)
{
	uintptr_t	address;

	if (!ptr)
	{
		*printed += write(1, "0x0", 3);
		return ;
	}
	address = (uintptr_t)ptr;
	*printed += write(1, "0x", 2);
	*printed += ft_convert_hexptr(address);
}
