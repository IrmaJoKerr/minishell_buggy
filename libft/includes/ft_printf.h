/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/21 18:31:48 by bleow             #+#    #+#             */
/*   Updated: 2024/09/24 23:24:33 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include "libft.h"
# include <stdarg.h>
# include <unistd.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>

int		ft_printf(const char *data, ...);
int		ft_putstr_rtn(const char *s, int fd);
void	master_parser(va_list *args, const char *data, unsigned int *printed);
void	ft_arg_c(char c, unsigned int *printed);
void	ft_arg_s(const char *s, unsigned int *printed);
void	ft_arg_di(int num, unsigned int *printed);
void	ft_arg_p(void *ptr, unsigned int *printed);
void	ft_arg_u(unsigned int num, unsigned int *printed);
void	ft_arg_x_lower(unsigned int hexlow, unsigned int *printed);
void	ft_arg_x_upper(unsigned int hexup, unsigned int *printed);
char	*ft_itoa(int n);

#endif