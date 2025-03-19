/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 17:52:35 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 01:22:19 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_putstr_fd outputs the string ’s’ to the given file descriptor.
*/

#include "libft.h"

void	ft_putstr_fd(char *s, int fd)
{
	if (!s)
		return ;
	while (*s != '\0')
	{
		write(fd, s, 1);
		s++;
	}
}
