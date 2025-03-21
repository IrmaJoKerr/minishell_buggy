/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 02:20:54 by bleow             #+#    #+#             */
/*   Updated: 2025/03/21 04:31:35 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Initialize a new AST state structure.
Allocates and sets up the AST building state.
*/
t_ast	*init_ast_struct(void)
{
	t_ast	*ast;
	
	ast = (t_ast *)malloc(sizeof(t_ast));
	if (!ast)
		return (NULL);
	ft_memset(ast, 0, sizeof(t_ast));
	ast->current = NULL;
    ast->last_cmd = NULL;
    ast->last_heredoc = NULL;
    ast->cmd_redir = NULL;
    ast->pipe_root = NULL;
    ast->root = NULL;
    ast->cmd_idx = 0;
    ast->syntax_error = 0;
    ast->serial_pipes = 0;
    ast->pipe_at_front = 0;
    ast->pipe_at_end = 0;
    ast->fd_write = 0;
    ast->expand_vars = 0;
	return (ast);
}

/*
Initialize execution context structure.
Returns:
- Newly allocated exec context.
- NULL on failure.
OLD VERSION
t_exec	*init_exec_context(t_vars *vars)
{
	t_exec	*exec;
	
	exec = (t_exec *)malloc(sizeof(t_exec));
	if (!exec)
		return (NULL);
	ft_memset(exec, 0, sizeof(t_exec));
	// If pipeline exists, copy any relevant values
	if (vars->pipeline)
		exec->append = vars->pipeline->append_mode;
	return (exec);
}
*/
t_exec	*init_exec_context(t_vars *vars)
{
    t_exec *exec;
    
    exec = (t_exec *)malloc(sizeof(t_exec));
    if (!exec)
        return (NULL);
    ft_memset(exec, 0, sizeof(t_exec));
    exec->cmd_path = NULL;
    exec->pid = 0;
    exec->status = 0;
    exec->result = 0;
    exec->cmd = NULL;
    if (vars && vars->pipeline)
        exec->append = vars->pipeline->append_mode;
    else
        exec->append = 0;
    return (exec);
}

/*
Initialize resources for input verification.
Allocates memory and initializes AST structure.
Returns:
- AST structure pointer on success
- NULL on failure (with *cmd_ptr set to NULL)
*/
t_ast	*init_verify(char *input, char **cmd_ptr)
{
	t_ast	*ast;
	
	fprintf(stderr, "DEBUG: Initializing verification resources\n");
	*cmd_ptr = ft_strdup(input);
	if (!*cmd_ptr)
		return (NULL);
	ast = init_ast_struct(); // Using NULL since we don't need vars here
	if (!ast)
	{
		ft_safefree((void **)cmd_ptr);
		return (NULL);
	}
	fprintf(stderr, "DEBUG: Verification resources initialized\n");
	return (ast);
}

/*
Initialize the lexer state.
Reset position trackers and node pointers.
*/
void	init_lexer(t_vars *vars)
{
	if (!vars)
		return;
	vars->head = NULL;
	vars->current = NULL;
	vars->curr_type = TYPE_NULL;
	vars->prev_type = TYPE_NULL;
	vars->pos = 0;
	vars->start = 0;
	vars->quote_depth = 0;
}
