/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:26:38 by bleow             #+#    #+#             */
/*   Updated: 2024/10/13 02:33:28 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_lstnew is a function that creates a new linked list node. Allocates
(with malloc(3)) and returns a new node. The member variable ’content’
is initialized with the value of the parameter ’content’. The variable
’next’ is initialized to NULL.
*/

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*node;

	node = malloc(sizeof(t_list));
	if (NULL == node)
		return (NULL);
	node->content = content;
	node->next = NULL;
	return (node);
}
