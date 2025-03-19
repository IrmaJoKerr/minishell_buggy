/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 19:53:49 by bleow             #+#    #+#             */
/*   Updated: 2024/10/16 19:56:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_putendl outputs the string ’s’ to the STDOUT followed by a newline.
*/

#include "libft.h"

void	ft_putendl(char *s)
{
	if (!s)
		return ;
	while (*s != '\0')
	{
		write(1, s, 1);
		s++;
	}
	write(1, "\n", 1);
}
