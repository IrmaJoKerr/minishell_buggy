/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:29:07 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:42:20 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_lstmap is a function that iterates the list ’lst’ and applies the function
’f’ on the content of each node. Creates a new list resulting of the
successive applications of the function ’f’. The ’del’ function is used to
delete the content of a node if needed.
*/

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*newlist;
	t_list	*newnode;

	if (!lst)
		return (NULL);
	newlist = NULL;
	while (lst)
	{
		if (f)
			newnode = ft_lstnew(f(lst->content));
		else
			newnode = ft_lstnew(lst->content);
		if (!newnode)
		{
			ft_lstclear(&newlist, del);
			return (NULL);
		}
		ft_lstadd_back(&newlist, newnode);
		lst = lst->next;
	}
	return (newlist);
}
