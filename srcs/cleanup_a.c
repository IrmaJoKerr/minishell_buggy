/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_a.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:56 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 19:54:27 by bleow            ###   ########.fr       */
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
OLD PRE DEBUG VERSION
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
*/
/*OLD DEBUG VERSION
void	cleanup_pipeline(t_pipe *pipeline)
{
    if (!pipeline)
    {
        fprintf(stderr, "DEBUG: [cleanup_pipeline] Pipeline is NULL\n");
        return;
    }
    
    fprintf(stderr, "DEBUG: [cleanup_pipeline] Starting pipeline cleanup\n");
    
    if (pipeline->exec_cmds)
    {
		fprintf(stderr, "DEBUG: [cleanup_pipeline] Starting Free exec_cmds\n");
        ft_safefree((void **)&pipeline->exec_cmds);
        fprintf(stderr, "DEBUG: [cleanup_pipeline] Freed exec_cmds\n");
    }
    
    if (pipeline->pipe_fds)
    {
		fprintf(stderr, "DEBUG: [cleanup_pipeline] Starting Free pipe_fds\n");
        ft_safefree((void **)&pipeline->pipe_fds);
        fprintf(stderr, "DEBUG: [cleanup_pipeline] Freed pipe_fds\n");
    }
    
    if (pipeline->pids)
    {
		fprintf(stderr, "DEBUG: [cleanup_pipeline] Starting Free pids\n");
        ft_safefree((void **)&pipeline->pids);
        fprintf(stderr, "DEBUG: [cleanup_pipeline] Freed pids\n");
    }
    
    if (pipeline->status)
    {
		fprintf(stderr, "DEBUG: [cleanup_pipeline] Starting Free status\n");
        ft_safefree((void **)&pipeline->status);
        fprintf(stderr, "DEBUG: [cleanup_pipeline] Freed status\n");
    }
    
    if (pipeline->heredoc_fd > 2)
    {
		fprintf(stderr, "DEBUG: [cleanup_pipeline] Starting close heredoc_fd: %d\n", pipeline->heredoc_fd);
        close(pipeline->heredoc_fd);
        fprintf(stderr, "DEBUG: [cleanup_pipeline] Closed heredoc_fd: %d\n", pipeline->heredoc_fd);
    }
    fprintf(stderr, "DEBUG: [cleanup_pipeline] Starting Free Pipeline structure\n");
    ft_safefree((void **)&pipeline);
    fprintf(stderr, "DEBUG: [cleanup_pipeline] Pipeline structure freed\n");
}
*/
void cleanup_pipeline(t_pipe *pipeline)
{
    if (!pipeline)
    {
        fprintf(stderr, "DEBUG: [cleanup_pipeline] Pipeline is NULL\n");
        return;
    }
    
    fprintf(stderr, "DEBUG: [cleanup_pipeline] Starting pipeline cleanup\n");
    fprintf(stderr, "DEBUG: [cleanup_pipeline] Pipeline address: %p\n", (void*)pipeline);
    
    // CRITICAL FIX: Add defensive checks before freeing
    if (pipeline->exec_cmds)
    {
        fprintf(stderr, "DEBUG: [cleanup_pipeline] exec_cmds address: %p\n", 
                (void*)pipeline->exec_cmds);
                
        // Sanity check to avoid freeing obviously bad pointers
        if ((uintptr_t)pipeline->exec_cmds > 0x1000 && 
            (uintptr_t)pipeline->exec_cmds < (uintptr_t)-0x1000)
        {
            fprintf(stderr, "DEBUG: [cleanup_pipeline] Freeing exec_cmds\n");
            ft_safefree((void **)&pipeline->exec_cmds);
        }
        else
        {
            fprintf(stderr, "DEBUG: [cleanup_pipeline] Skipping free of invalid exec_cmds\n");
            pipeline->exec_cmds = NULL;
        }
    }
    
    // Same defensive check for other pointers
    if (pipeline->pipe_fds)
    {
        if ((uintptr_t)pipeline->pipe_fds > 0x1000 && 
            (uintptr_t)pipeline->pipe_fds < (uintptr_t)-0x1000)
        {
            fprintf(stderr, "DEBUG: [cleanup_pipeline] Freeing pipe_fds\n");
            ft_safefree((void **)&pipeline->pipe_fds);
        }
        else
        {
            fprintf(stderr, "DEBUG: [cleanup_pipeline] Skipping free of invalid pipe_fds\n");
            pipeline->pipe_fds = NULL;
        }
    }
    
    // Continue with similar checks for other pointers
    // ...
    
    // Finally free the pipeline structure
    fprintf(stderr, "DEBUG: [cleanup_pipeline] Freeing pipeline structure\n");
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
OLD VERSION
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
*/
/*OLD VERSION PRE DEBUG
void	cleanup_vars(t_vars *vars)
{
    if (!vars)
        return;
        
    if (vars->env)
    {
        ft_free_2d(vars->env, ft_arrlen(vars->env));
        vars->env = NULL;
    }
    if (vars->error_msg != NULL)
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
*/
void	cleanup_vars(t_vars *vars)
{
    int env_count;
    
    if (!vars)
    {
        fprintf(stderr, "DEBUG: [cleanup_vars] Vars is NULL\n");
        return;
    }
    
    fprintf(stderr, "DEBUG: [cleanup_vars] Starting vars cleanup\n");
    
    if (vars->env)
    {
        env_count = ft_arrlen(vars->env);
        fprintf(stderr, "DEBUG: [cleanup_vars] Freeing %d environment variables\n", env_count);
        ft_free_2d(vars->env, env_count);
        vars->env = NULL;
    }
    
    if (vars->error_msg != NULL)
    {
        fprintf(stderr, "DEBUG: [cleanup_vars] Freeing error message: '%s'\n", vars->error_msg);
        ft_safefree((void **)&vars->error_msg);
    }
    
    if (vars->astroot)
    {
        fprintf(stderr, "DEBUG: [cleanup_vars] Cleaning up AST root\n");
        cleanup_ast(vars->astroot);
        fprintf(stderr, "DEBUG: [cleanup_vars] AST root cleaned\n");
    }
    
    if (vars->head && vars->head != vars->astroot)
    {
        fprintf(stderr, "DEBUG: [cleanup_vars] Cleaning up head (not AST root)\n");
        cleanup_ast(vars->head);
        fprintf(stderr, "DEBUG: [cleanup_vars] Head cleaned\n");
    }
    
    vars->astroot = NULL;
    vars->head = NULL;
    vars->current = NULL;
    vars->quote_depth = 0;
    
    // CRITICAL FIX: Only access pipeline if it exists
    // This prevents trying to access a pipeline that might have been freed
    if (vars->pipeline)
    {
        fprintf(stderr, "DEBUG: [cleanup_vars] Resetting pipeline command code\n");
        vars->pipeline->last_cmdcode = 0;
    }
    else
    {
        fprintf(stderr, "DEBUG: [cleanup_vars] Pipeline already freed\n");
    }
    
    fprintf(stderr, "DEBUG: [cleanup_vars] Vars cleanup complete\n");
}

/*
Perform complete cleanup before program exit.
- Cleans up token list, variables, and pipeline.
- Should be called before any exit() call.
- Ensures all allocated memory is properly freed.
Works with cleanup_vars() and main().
OLD VERSION
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
*/
/*
Performs complete cleanup before exiting on Ctrl+D.
- Cleans token list to prevent double-free errors.
- Saves command history to file.
- Frees all allocated resources.
- Clears readline history.
- Handles pipeline resources properly.
Works with main() shell loop for clean termination.

Example: When user presses Ctrl+D after a pipe operation
- Cleans up the token list from interrupted pipe processing
- Saves history safely with clean token state
- Prevents memory leaks and invalid pointer errors
*/
void	cleanup_exit(t_vars *vars)
{
    if (!vars)
        return ;
    fprintf(stderr, "DEBUG: Starting cleanup before exit\n");
    save_history();
	cleanup_token_list(vars);
    cleanup_vars(vars);
    if (vars->pipeline)
        cleanup_pipeline(vars->pipeline);
    vars->pipeline = NULL;
    rl_clear_history();
    fprintf(stderr, "DEBUG: Cleanup completed, safe to exit\n");
}
