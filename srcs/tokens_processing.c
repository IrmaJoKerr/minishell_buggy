/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_processing.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 17:45:54 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 15:37:11 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Process non-command tokens
*/
void	process_other_token(char *input, t_vars *vars)
{
	char	*token;
	t_node	*node;
	if (vars->pos <= vars->start)
		return ;
	token = ft_substr(input, vars->start, vars->pos - vars->start);
	if (!token)
		return ;
	if (vars->curr_type == TYPE_ARGS)
	{
		node = make_cmdnode(token);
		// Token must be freed in all cases
		ft_safefree((void **)&token);
		if (!node)
			return ;
	}
	else
	{
		node = new_other_node(token, vars->curr_type);
		if (!node)
			return ;
	}
	if (vars->curr_type != TYPE_CMD && vars->current)
		add_child(vars->current, node);
	else
		vars->current = node;
}

/*
Process command tokens by splitting input string
with whitespace as delimiter. Uses ft_splitstr()
to handle quotes during splitting.
*/
void    process_cmd_token(char *input, t_vars *vars)
{
	char    **args;
	t_node  *node;

	if (!input || !vars)
		return ;
	args = ft_splitstr(input, " \t\n\v\f\r");
	if (!args)
		return ;
	// Process the arguments
	process_args_tokens(args);
	debug_cmd_tokens(args); // Debug print
	// Create node directly in this function
	if (args[0])
	{
		node = build_cmdarg_node(args);
		if (node)
			build_token_linklist(vars, node);
	}
	ft_free_2d(args, ft_arrlen(args));
}

/*
Process quotes and handle dash arguments
*/
void    process_args_tokens(char **args)
{
	int     i;

	i = 0;
	while (args[i])
	{
		process_quotes_in_arg(&args[i]);
		// Handle dash arguments
		if (is_flag_arg(args, i))
			join_flag_args(args, i);
		else
			i++;
	}
}

/*
Process pipe token.
Creates a pipe token node and adds it to the token list.
*/
void process_pipe_token(t_vars *vars)
{
    t_node	*pipe_node;
    char	*pipe_str;
	t_node	*last;
    
    pipe_str = ft_strdup("|");
    if (!pipe_str)
        return ;
    pipe_node = initnode(TYPE_PIPE, pipe_str);
    ft_safefree((void **)&pipe_str);
    if (!pipe_node)
        return ;
    if (vars->head)
    {
        if (vars->current)
        {
            pipe_node->next = vars->current->next;
            if (vars->current->next)
                vars->current->next->prev = pipe_node;
            vars->current->next = pipe_node;
            pipe_node->prev = vars->current;
            vars->current = pipe_node;
        }
        else
        {
            last = vars->head;
            while (last->next)
                last = last->next;
            last->next = pipe_node;
            pipe_node->prev = last;
            vars->current = pipe_node;
        }
    }
    else
    {
        vars->head = pipe_node;
        vars->current = pipe_node;
    }
    fprintf(stderr, "DEBUG: Added pipe token to list\n");
}
