/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 17:44:57 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 06:37:14 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check if the first token in the list is a pipe (syntax error).
Code 258 is the standard code used for syntax errors.
Returns:
0 - no pipe at beginning
1 - pipe at beginning (sets error code)
*/
int	check_initial_pipe(t_vars *vars, t_ast *ast)
{
	if (!vars || !vars->head || !ast)
		return (0);       
	/* Check if the first token is a pipe */
	if (vars->head->type == TYPE_PIPE)
	{
		fprintf(stderr, "DEBUG: Syntax error: pipe at beginning\n");
		if (vars->pipeline)
			vars->pipeline->last_cmdcode = 258;
		ast->pipe_at_front = 1;
		ast->syntax_error = 1;
		return (1);
	}
	return (0);
}

/*
Check for consecutive pipes in token list (syntax error).
Code 258 is the standard code used for syntax errors.
Returns:
0 - no consecutive pipes found
1 - consecutive pipes found (sets error code)
*/
int	check_serial_pipes(t_vars *vars, t_ast *ast)
{
	t_node	*current;
	
	if (!vars || !vars->head || !ast)
		return (0);
	ast->serial_pipes = 0;
	current = vars->head;
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			if (ast->serial_pipes > 0)
			{
				fprintf(stderr, "DEBUG: Syntax error: consecutive pipes\n");
				if (vars->pipeline)
					vars->pipeline->last_cmdcode = 258;
				ast->syntax_error = 1;
				return (1);
			}
			ast->serial_pipes++;
		}
		else
			ast->serial_pipes = 0;
		current = current->next;
	}
	return (0);
}

/*
Check for initial/consecutive pipe syntax errors.
Returns:
0 - no initial/consecutive pipe errors
1 - found syntax error (sets error code)
*/
int	check_bad_pipe_series(t_vars *vars)
{
	int		result;
	t_ast	*ast;
	
	ast = init_ast_struct();
	if (!ast)
		return (0);
	// First check for a pipe at the beginning
	result = check_initial_pipe(vars, ast);
	if (result != 0)
	{
		cleanup_ast_struct(ast);
		return (result);
	}
	// Then check for consecutive pipes
	result = check_serial_pipes(vars, ast);
	if (result != 0)
	{
		cleanup_ast_struct(ast);
		return (result);
	}
	cleanup_ast_struct(ast);
	return (0);
}

/*
Builds an AST from the token linked list in vars.
This is a sub-control function for the AST building process by:
- Initializing the AST structure
- Finding and collecting all command nodes from the token list
- Processing pipe nodes to create the pipeline structure
- Handling all redirection nodes and their connections
- Determining the root node based on the constructed tree
Returns:
- The root node of the completed AST, ready for execution
- NULL if AST creation fails
OLD VERSION
t_node	*process_token_list(t_vars *vars)
{
	t_ast	ast;
	t_node	*root;
	
	ft_memset(&ast, 0, sizeof(t_ast));
	// Collect command nodes from token list
	find_cmd_nodes(vars);
	// Process pipe nodes
	ast.pipe_root = process_pipe_nodes(vars, &ast);
	// Process redirection nodes
	process_redirections(vars, &ast);
	// Determine root node
	root = set_ast_root(ast.pipe_root, vars);
	return (root);
}
*/
t_node	*process_token_list(t_vars *vars)
{
    t_ast	ast;
    t_node	*root;
    t_node	*pipe_root = NULL;
    
    ft_memset(&ast, 0, sizeof(t_ast));
    // Collect command nodes from token list
    find_cmd_nodes(vars);
    // Process pipe nodes
    pipe_root = process_pipe_nodes(vars, &ast);
    ast.pipe_root = pipe_root;
    // Process redirection nodes
    process_redirections(vars, &pipe_root, &ast);
    // Determine root node
    root = set_ast_root(pipe_root, vars);
    return (root);
}
