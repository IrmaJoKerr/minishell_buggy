/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   master_parser.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/20 19:51:38 by bleow             #+#    #+#             */
/*   Updated: 2024/09/24 21:11:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/ft_printf.h"

int	ft_putstr_rtn(const char *s, int fd)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i] != '\0')
	{
		write(fd, &s[i], 1);
		i++;
	}
	return (i);
}

void	master_parser(va_list *args, const char *data, unsigned int *printed)
{
	if (*data == '%')
	{
		*printed += write(1, "%", 1);
	}
	else if (*data == 'c')
		ft_arg_c(va_arg(*args, int), printed);
	else if (*data == 's')
		ft_arg_s(va_arg(*args, const char *), printed);
	else if ((*data == 'i') || (*data == 'd'))
		ft_arg_di(va_arg(*args, int), printed);
	else if (*data == 'p')
		ft_arg_p(va_arg(*args, void *), printed);
	else if (*data == 'u')
		ft_arg_u(va_arg(*args, unsigned int), printed);
	else if (*data == 'x')
		ft_arg_x_lower(va_arg(*args, unsigned int), printed);
	else if (*data == 'X')
		ft_arg_x_upper(va_arg(*args, unsigned int), printed);
}
