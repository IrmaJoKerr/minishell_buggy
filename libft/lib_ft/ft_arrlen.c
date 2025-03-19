/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_arrlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 02:26:32 by bleow             #+#    #+#             */
/*   Updated: 2024/10/15 07:45:04 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_arrlen is a function that returns the length of a 2D char array.
*/

#include "libft.h"

size_t	ft_arrlen(char **array)
{
	size_t	count;

	count = 0;
	if (!array)
		return (0);
	while (array[count] != NULL)
		count++;
	return (count);
}
