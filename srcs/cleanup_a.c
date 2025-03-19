/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_a.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:56 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 18:04:56 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Frees a partially allocated environment array up to index n-1.
- Takes the environment array and number of elements to free.
- Safely frees each string element, then frees the array itself.
- Prevents memory leaks during initialization errors.
Works with ft_free_2d() and dup_env().
*/
void	cleanup_env_error(char **env, int n)
{
    while (n > 0)
    {
        n--;
        ft_safefree((void **)&env[n]);
    }
    ft_safefree((void **)&env);
}

/*
Cleanup AST struct variables.
- Takes a pointer to an AST structure.
- Safely frees the structure itself.
- Prevents memory leaks after AST processing.
Works with init_ast_struct().
*/
void	cleanup_ast_struct(t_ast *ast)
{
    if (!ast)
        return ;
    ft_safefree((void **)&ast);
}

/*
Cleanup pipeline assets.
- Takes a pointer to a pipeline structure.
- Frees all dynamically allocated arrays inside the pipeline.
- Closes any open heredoc file descriptors.
- Finally frees the pipeline structure itself.
- Prevents memory leaks after pipeline execution.
Works with init_pipeline_struct().

Example: After executing "cat file1 | grep pattern | wc -l":
- Frees exec_cmds, pipe_fds, pids, and status arrays
- Closes any open heredoc file descriptor
- Frees the pipeline structure
*/
void	cleanup_pipeline(t_pipe *pipeline)
{
    if (!pipeline)
        return ;
    if (pipeline->exec_cmds)
        ft_safefree((void **)&pipeline->exec_cmds);
    if (pipeline->pipe_fds)
        ft_safefree((void **)&pipeline->pipe_fds);
    if (pipeline->pids)
        ft_safefree((void **)&pipeline->pids);
    if (pipeline->status)
        ft_safefree((void **)&pipeline->status);
    if (pipeline->heredoc_fd > 2)
        close(pipeline->heredoc_fd);
    ft_safefree((void **)&pipeline);
}

/*
Cleanup function to free allocated memory in s_vars struct.
- Frees environment variables array.
- Frees error messages.
- Cleans up AST structures.
- Resets status variables.
- Prevents memory leaks during normal operation and errors.
Works with cleanup_token_list() and cleanup_exit().
*/
void	cleanup_vars(t_vars *vars)
{
    if (!vars)
        return ;
    if (vars->env)
    {
        ft_free_2d(vars->env, ft_arrlen(vars->env));
        vars->env = NULL;
    }
    ft_safefree((void **)&vars->error_msg);
    if (vars->astroot)
        cleanup_ast(vars->astroot);
    if (vars->head && vars->head != vars->astroot)
        cleanup_ast(vars->head);
    vars->astroot = NULL;
    vars->head = NULL;
    vars->current = NULL;
    vars->quote_depth = 0;
    if (vars->pipeline)
        vars->pipeline->last_cmdcode = 0;
}

/*
Perform complete cleanup before program exit.
- Cleans up token list, variables, and pipeline.
- Should be called before any exit() call.
- Ensures all allocated memory is properly freed.
Works with cleanup_vars() and main().
*/
void	cleanup_exit(t_vars *vars)
{
    if (!vars)
        return ;
    cleanup_token_list(vars); 
    cleanup_vars(vars);
    if (vars->pipeline)
        cleanup_pipeline(vars->pipeline);
    vars->pipeline = NULL;
}
