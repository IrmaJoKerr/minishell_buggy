/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initnode.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 15:53:06 by bleow             #+#    #+#             */
/*   Updated: 2025/03/18 23:34:27 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets up basic node structure framework.
Handles node type, token, and argument array initialization.
Has special case for head nodes.
Sets a default token if token is NULL.
Returns 1 on success, 0 on failure.
*/
int	make_nodeframe(t_node *node, t_tokentype type, char *token)
{
	node->type = type;
	node->next = NULL;
	node->prev = NULL;
	node->left = NULL;
	node->right = NULL;
	
	if (!token)
		token = (char *)get_token_str(type);
	create_args_array(node, token);
	if (!node->args)
		return (0);
	return (1);
}

/*
Creates a new node with specified type and token.
If token is NULL, uses appropriate defaults based on type.
Returns the initialized node or NULL on failure.
*/
t_node *initnode(t_tokentype type, char *token)
{
	t_node *node;
	
	node = malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	if (!make_nodeframe(node, type, token))
	{
		ft_safefree((void **)&node);
		return (NULL);
	}
	return (node);
}

