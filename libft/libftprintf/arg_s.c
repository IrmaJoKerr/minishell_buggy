/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_s.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/20 19:50:14 by bleow             #+#    #+#             */
/*   Updated: 2024/09/24 21:11:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/ft_printf.h"

void	ft_arg_s(const char *s, unsigned int *printed)
{
	int			i;

	if (!s)
	{
		*printed += write(1, "(null)", 6);
		return ;
	}
	i = 0;
	while (s[i] != '\0')
	{
		write(1, &s[i], 1);
		i++;
	}
	*printed += i;
}
