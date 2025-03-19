/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:28:38 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 03:09:19 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_lstclear is a function that deletes and frees the given node and every
successor of that node, using the function ’del’ and free(3). Finally, the
pointer to the list is set to NULL.
*/

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*current;
	t_list	*tmp;

	if (!lst || !del)
		return ;
	current = *lst;
	while (current != NULL)
	{
		tmp = current;
		current = current->next;
		del(tmp->content);
		ft_safefree((void **)&tmp);
	}
	*lst = NULL;
}
