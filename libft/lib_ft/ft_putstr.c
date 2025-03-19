/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 19:51:38 by bleow             #+#    #+#             */
/*   Updated: 2024/10/16 19:55:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_putstr outputs the string ’s’ to the STDOUT.
*/

#include "libft.h"

void	ft_putstr(char *s)
{
	if (!s)
		return ;
	while (*s != '\0')
	{
		write(1, s, 1);
		s++;
	}
}
