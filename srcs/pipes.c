/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 09:52:41 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 05:30:25 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Configures pipe redirections for command execution.
- Checks if command is part of a pipe node.
- Closes read end of pipe to avoid descriptor leaks.
- Redirects STDOUT to pipe write end for data flow.
- Ensures proper cleanup of file descriptors.
Returns:
Nothing (void function).
Works with execute_pipe_command() during pipeline execution.

Example: For first command in "ls | grep txt"
- Redirects stdout to pipe
- Output of "ls" flows through pipe to "grep"
- Ensures file descriptors are properly managed
*/
void	init_pipe(t_node *cmd, int *pipe_fd)
{
    if (cmd->type == TYPE_PIPE)
    {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
    }
}

/*
Validates pipe node structure before execution.
- Checks that node is a valid pipe node.
- Ensures both left and right branches exist.
- Verifies node has proper type.
Returns:
1 if valid, 0 if invalid.
Works with execute_pipeline() for error checking.

Example: Before executing "cmd1 | cmd2"
- Checks that pipe node has TYPE_PIPE
- Verifies left branch (cmd1) exists
- Verifies right branch (cmd2) exists
*/
int	validate_pipe_node(t_node *pipe_node)
{
    if (!pipe_node || pipe_node->type != TYPE_PIPE)
        return (0);
    if (!pipe_node->left || !pipe_node->right)
        return (0);
    return (1);
}

/*
Creates and sets up pipe for command communication.
- Initializes pipe file descriptors.
- Handles pipe creation errors.
Returns:
1 on success, 0 on failure.
Works with execute_pipeline() for pipe setup.

Example: Before forking processes
- Creates pipe with read and write ends
- Reports errors if pipe creation fails
- Returns success/failure status
*/
int	setup_pipe(int *pipefd)
{
    if (pipe(pipefd) == -1)
    {
        ft_putendl_fd("pipe: Creation failed", 2);
        return (0);
    }
    return (1);
}

/*
Executes left side of pipe in child process.
- Closes unused pipe ends.
- Redirects stdout to pipe write end.
- Executes left command or sub-pipeline.
Returns:
Never returns (calls exit).
Works with execute_pipeline() for left command.

Example: For "ls | grep txt"
- Creates child process for "ls"
- Redirects output to pipe
- Executes "ls" command
- Exits with command status
*/
void	exec_left_cmd(t_node *pipe_node, int *pipefd, t_vars *vars)
{
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
    exit(execute_cmd(pipe_node->left, vars->env, vars));
}

/*
Executes right side of pipe in child process.
- Closes unused pipe ends.
- Redirects stdin to pipe read end.
- Executes right command or sub-pipeline.
Returns:
Never returns (calls exit).
Works with execute_pipeline() for right command.

Example: For "ls | grep txt"
- Creates child process for "grep txt"
- Redirects input from pipe
- Executes "grep" command
- Exits with command status
*/
void	exec_right_cmd(t_node *pipe_node, int *pipefd, t_vars *vars)
{
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    exit(execute_cmd(pipe_node->right, vars->env, vars));
}

/*
Creates child process for a pipeline command.
- Forks new process.
- Handles fork errors.
- Executes specified command in child.
Returns:
Process ID on success, -1 on failure.
Works with execute_pipeline() for process creation.

Example: Creating process for command in pipeline
- Forks new process
- Child executes command function
- Parent returns child's PID
- Returns -1 if fork fails
*/
pid_t	make_child_proc(t_node *pipe_node, int *pipefd, 
        t_vars *vars, int is_left)
{
    pid_t	pid;

    pid = fork();
    if (pid == 0)
    {
        if (is_left)
            exec_left_cmd(pipe_node, pipefd, vars);
        else
            exec_right_cmd(pipe_node, pipefd, vars);
    }
    else if (pid < 0)
        ft_putendl_fd("fork: Creation failed", 2);
    return (pid);
}

/*
Cleans up resources allocated during pipe completion processing.
- Frees AST structure if provided.
- Releases pipe command buffer if requested.
- Frees result buffer if requested.
- Centralizes cleanup to avoid code duplication.
Works with handle_pipe_completion().
*/
void	reset_done_pipes(t_ast *ast, char **pipe_cmd, char **result,
				int free_flags)
{
    if (ast)
        cleanup_ast_struct(ast);
    if ((free_flags & 1) && pipe_cmd && *pipe_cmd)
        ft_safefree((void **)pipe_cmd);
    if ((free_flags & 2) && result && *result)
        ft_safefree((void **)result);
}

/*
Prepares and validates a command for pipe completion processing.
- Creates working copies of command string.
- Checks if command needs completion based on syntax flag.
- Creates AST structure for processing.
Returns:
- 1 on successful preparation.
- 0 if no completion needed (early return case).
- -1 on preparation errors.
Works with handle_pipe_completion().
*/
int prep_pipe_complete(char *cmd, char **result, char **pipe_cmd, t_ast **ast)
{
    *result = ft_strdup(cmd);
    if (!*result)
        return (-1);
    *pipe_cmd = ft_strdup(*result);
    if (!*pipe_cmd)
    {
        ft_safefree((void **)result);
        return (-1);
    }
    *ast = init_ast_struct();
    if (!*ast)
    {
        ft_safefree((void **)result);
        ft_safefree((void **)pipe_cmd);
        return (-1);
    }
    return (1);
}

/*
Handles pipe completion for commands needing continuation.
- Processes commands with trailing pipes.
- Prompts for additional input as needed.
Returns:
- Complete command string or NULL on error.
Works with process_pipe_syntax().
Example: For "ls |" (incomplete pipe)
- Returns: "ls | grep hello" after user inputs "grep hello"
*/
char *handle_pipe_completion(char *cmd, t_vars *vars, int syntax_chk)
{
    char    *pipe_cmd;
    char    *result;
    t_ast   *ast;
    int     prep_status;
    
    if (!syntax_chk || is_input_complete(vars))
        return (ft_strdup(cmd));
    prep_status = prep_pipe_complete(cmd, &result, &pipe_cmd, &ast);
    if (prep_status < 0)
        return (NULL);
    if (prep_status == 0)
        return (ft_strdup(cmd));
    if (handle_unfinished_pipes(&pipe_cmd, vars, ast) < 0)
    {
        reset_done_pipes(ast, &pipe_cmd, &result, 3);
        return (NULL);
    }
    reset_done_pipes(ast, NULL, &result, 2);
    return (pipe_cmd);
}

/*
Sets up and launches child processes for a pipeline.
- Creates pipe for command communication.
- Launches left and right child processes.
- Handles fork and pipe errors appropriately.
Returns:
0 on success with pids set, 1 on error.
Works with run_pipeline() during pipeline execution.

Example: For "ls | grep txt" pipeline
- Creates pipe between processes
- Launches child processes for both commands
- Sets process IDs in left_pid and right_pid
*/
int	setup_pipeline_procs(t_node *pipe_node, t_vars *vars, 
	pid_t *left_pid, pid_t *right_pid)
{
	int	pipefd[2];

	if (!validate_pipe_node(pipe_node))
		return (1);
	if (!setup_pipe(pipefd))
		return (1);
	*left_pid = make_child_proc(pipe_node, pipefd, vars, 1);
	if (*left_pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (1);
	}
	*right_pid = make_child_proc(pipe_node, pipefd, vars, 0);
	if (*right_pid < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(*left_pid, NULL, 0);
		return (1);
	}
	close(pipefd[0]);
	close(pipefd[1]);
	return (0);
}

/*
Executes commands connected by a pipe.
- Sets up and launches pipeline child processes.
- Waits for both processes to complete.
- Captures status of command execution.
Returns:
- Status of right command.
- 1 on error.
Works with execute_cmd() for pipeline execution.

Example: For "ls -l | grep txt"
- Sets up pipeline processes
- Waits for both commands to complete
- Returns final execution status
*/
int	execute_pipeline(t_node *pipe_node, t_vars *vars)
{
pid_t	left_pid;
pid_t	right_pid;
int		status;

if (setup_pipeline_procs(pipe_node, vars, &left_pid, &right_pid))
	return (1);
waitpid(left_pid, NULL, 0);
waitpid(right_pid, &status, 0);
return (handle_cmd_status(status, vars));
}
