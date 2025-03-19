/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arguments.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:36:41 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 03:33:40 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Create an array of arguments(flags) for the node.
Works like malloc for the array.
Example: "ls" -> args array: ["ls", NULL]
*/
void	create_args_array(t_node *node, char *token)
{
	char	**args;

	args = malloc(sizeof(char *) * 2);
	if (!args)
		return ;
	args[0] = ft_strdup(token);
	if (!args[0])
	{
		free(args);
		return ;
	}
	args[1] = NULL;
	node->args = args;
}

/*
Append a new argument to the node's argument array.
Works like realloc for the array.
Example: node->args is ["ls", "-l", NULL]
After append_arg(node, "-a"), node->args becomes ["ls", "-l", "-a", NULL]
*/
void	append_arg(t_node *node, char *new_arg)
{
	char	**new_args;
	size_t	len;
	size_t	i;

	if (!node || !new_arg || !node->args)
		return ;
	len = ft_arrlen(node->args);
	new_args = malloc(sizeof(char *) * (len + 2));
	if (!new_args)
		return ;
	i = -1;
	while (++i < len)
	{
		new_args[i] = node->args[i];
		if (!new_args[i])
		{
			free(new_args);
			return ;
		}
	}
	new_args[len] = ft_strdup(new_arg);
	new_args[len + 1] = NULL;
	free(node->args);
	node->args = new_args;
}
