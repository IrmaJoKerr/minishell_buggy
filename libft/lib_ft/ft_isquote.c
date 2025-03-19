/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isquote.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 00:10:30 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 12:15:48 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_isquote() functions tests whether c is a quote character.
The function returns 1 if c is a quote character; otherwise, it returns 0.
*/
#include "libft.h"

int	ft_isquote(char c)
{
	if (c == '\'' || c == '\"')
		return (1);
	else
		return (0);
}
