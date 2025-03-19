/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/21 18:31:31 by bleow             #+#    #+#             */
/*   Updated: 2024/09/24 21:11:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/ft_printf.h"

int	ft_printf(const char *data, ...)
{
	va_list			args;
	unsigned int	len;
	unsigned int	printed;

	len = 0;
	printed = 0;
	if (!data)
		return (-1);
	va_start(args, data);
	while (data[len] != '\0')
	{
		if (data[len] == '%')
		{
			len++;
			master_parser(&args, &data[len], &printed);
		}
		else
			printed += write(1, &data[len], 1);
		len++;
	}
	va_end(args);
	return (printed);
}
