/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:28:14 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 03:09:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_lstdelone is a function that deletes a single element/node from a list. 
It takes as a parameter a node and frees the memory of the node’s content
using the function ’del’ given as a parameter and free the node. The memory of
’next’ is NOT freed.
*/

#include "libft.h"

void	ft_lstdelone(t_list *lst, void (*del)(void *))
{
	if (!lst || !del)
		return ;
	del(lst->content);
	ft_safefree((void **)&lst);
}
