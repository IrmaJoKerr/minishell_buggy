/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 04:07:10 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles command execution status and updates error code.
- Processes exit status from waitpid() for child processes.
- For normal exits, stores the exit code (0-255) directly.
- For signals, adds 128 to the signal number (POSIX standard).
Returns:
The final error code stored in vars->error_code.
Works with exec_child_cmd() and execute_pipeline().

Example: Child process terminated by SIGINT (signal 2)
- Sets vars->error_code to 130 (128+2)
- Returns 130
*/
int	handle_cmd_status(int status, t_vars *vars)
{
    if (WIFEXITED(status))
        vars->error_code = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        vars->error_code = WTERMSIG(status) + 128;
    return (vars->error_code);
}

/*
Handles redirection setup for output files.
- Opens file for writing in truncate or append mode.
- Redirects stdout to the opened file.
- Properly handles and reports errors.
Returns:
1 on success, 0 on failure.
Works with setup_redirection().
*/
int	setup_out_redir(t_node *node, int *fd, int append)
{
    int	flags;

    flags = O_WRONLY | O_CREAT;
    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    fprintf(stderr, "DEBUG: Opening '%s' for output redirection\n", 
        node->right->args[0]);
    *fd = open(node->right->args[0], flags, 0644);
    if (*fd == -1)
    {
        fprintf(stderr, "DEBUG: Failed to open file '%s'\n", 
            node->right->args[0]);
        perror("open");
        return (0);
    }
    fprintf(stderr, "DEBUG: Successfully opened file, fd=%d\n", *fd);
    if (dup2(*fd, STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG: dup2 failed for stdout redirection\n");
        perror("dup2");
        return (0);
    }
    return (1);
}

/*
Handles redirection setup for input files.
- Opens file for reading.
- Redirects stdin to read from the file.
- Properly handles and reports errors.
Returns:
1 on success, 0 on failure.
Works with setup_redirection().
*/
int	setup_in_redir(t_node *node, int *fd)
{
    fprintf(stderr, "DEBUG: Opening '%s' for input redirection\n", 
        node->right->args[0]);
    *fd = open(node->right->args[0], O_RDONLY);
    if (*fd == -1)
    {
        fprintf(stderr, "DEBUG: Failed to open file '%s'\n", 
            node->right->args[0]);
        perror("open");
        return (0);
    }
    fprintf(stderr, "DEBUG: Successfully opened file for reading\n");
    if (dup2(*fd, STDIN_FILENO) == -1)
    {
        fprintf(stderr, "DEBUG: dup2 failed for stdin redirection\n");
        perror("dup2");
        return (0);
    }
    return (1);
}

/*
Sets up appropriate redirection based on node type.
- Handles all redirection types (input, output, append, heredoc).
- Creates or opens files with appropriate permissions.
- Redirects stdin/stdout as needed.
Returns:
1 on success, 0 on failure.
Works with exec_redirect_cmd().
*/
int	setup_redirection(t_node *node, t_vars *vars, int *fd)
{
    if (node->type == TYPE_OUT_REDIRECT)
        return (setup_out_redir(node, fd, 0));
    else if (node->type == TYPE_APPEND_REDIRECT)
        return (setup_out_redir(node, fd, 1));
    else if (node->type == TYPE_IN_REDIRECT)
        return (setup_in_redir(node, fd));
    else if (node->type == TYPE_HEREDOC)
    {
        fprintf(stderr, "DEBUG: Processing heredoc\n");
        return (proc_heredoc(node, vars));
    }
    return (0);
}

/*
Executes a command with redirection.
- Saves original file descriptors.
- Sets up redirection according to node type.
- Executes the command with redirection in place.
- Restores original file descriptors afterward.
Returns:
Result of command execution.
Works with execute_cmd().
*/
int	exec_redirect_cmd(t_node *node, char **envp, t_vars *vars)
{
    int	saved_stdout;
    int	saved_stdin;
    int	fd;
    int	result;

    fprintf(stderr, "DEBUG: Executing redirection %s\n", 
        get_token_str(node->type));
    if (!node->left || !node->right)
    {
        fprintf(stderr, "DEBUG: Invalid redirection node structure\n");
        return (1);
    }
    saved_stdout = dup(STDOUT_FILENO);
    saved_stdin = dup(STDIN_FILENO);
    fd = -1;
    if (!setup_redirection(node, vars, &fd))
        return (1);
    result = execute_cmd(node->left, envp, vars);
    fprintf(stderr, "DEBUG: Restoring original file descriptors\n");
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stdin, STDIN_FILENO);
    cleanup_fds(saved_stdout, saved_stdin);
    if (fd > 2)
        close(fd);
    return (result);
}

/*
Executes a child process for external commands.
- Forks a child process.
- In child: executes the external command.
- In parent: waits for child and processes exit status.
Returns:
Exit code from the command execution.
Works with execute_cmd().
*/
int	exec_child_cmd(t_node *node, char **envp, t_vars *vars, char *cmd_path)
{
    pid_t	pid;
    int		status;

    pid = fork();
    if (pid == 0)
    {
        fprintf(stderr, "DEBUG: Child process executing: %s\n", cmd_path);
        if (execve(cmd_path, node->args, envp) == -1)
        {
            perror("bleshell");
            exit(1);
        }
    }
    else if (pid < 0)
    {
        perror("bleshell: fork");
        ft_safefree((void **)&cmd_path);
        return (1);
    }
    else
    {
        waitpid(pid, &status, 0);
        ft_safefree((void **)&cmd_path);
        return (handle_cmd_status(status, vars));
    }
    return (0);
}

/*
Debug prints command arguments.
- Prints each argument with proper formatting.
- Useful for tracing command execution.
Works with execute_cmd().
*/
void	print_cmd_args(t_node *node)
{
    int	i;

    fprintf(stderr, "DEBUG: Command: '%s' with %ld arguments\n", 
        node->args[0], ft_arrlen(node->args) - 1);
    i = 0;
    fprintf(stderr, "DEBUG: Arguments: ");
    while (node->args[i])
    {
        fprintf(stderr, "'%s'%s", node->args[i], 
            node->args[i + 1] ? ", " : "");
        i++;
    }
    fprintf(stderr, "\n");
}

/*
Handles standard command execution.
- Expands command arguments (variables, etc).
- Checks if command is a builtin and handles accordingly.
- For external commands: finds path and executes.
Returns:
Exit code from the command execution.
Works with execute_cmd().
*/
int	exec_std_cmd(t_node *node, char **envp, t_vars *vars)
{
    char	*cmd_path;

    if (!node->args || !node->args[0])
    {
        fprintf(stderr, "DEBUG: Invalid command node or missing arguments\n");
        return (1);
    }
    expand_cmd_args(node, vars);
    print_cmd_args(node);
    if (is_builtin(node->args[0]))
    {
        fprintf(stderr, "DEBUG: Executing builtin command: %s\n", 
            node->args[0]);
        return (execute_builtin(node->args[0], node->args, vars));
    }
    cmd_path = get_cmd_path(node->args[0], envp);
    if (!cmd_path)
    {
        ft_putstr_fd("bleshell: command not found: ", 2);
        ft_putendl_fd(node->args[0], 2);
        vars->error_code = 127;
        return (vars->error_code);
    }
    fprintf(stderr, "DEBUG: Found command path: %s\n", cmd_path);
    return (exec_child_cmd(node, envp, vars, cmd_path));
}

/*
Master function to execute commands based on node type.
- Handles various node types (cmd, pipe, redirections).
- Routes execution to appropriate handlers.
- Ensures proper cleanup after execution.
Returns:
Exit status of the executed command.
Works with process_command() in main execution loop.

Example: For input "ls -l | grep foo > output.txt":
- First executes the pipeline with ls and grep
- Handles the redirection to output.txt
- Returns final exit status
*/
int	execute_cmd(t_node *node, char **envp, t_vars *vars)
{
    if (!node)
    {
        fprintf(stderr, "DEBUG: NULL command node\n");
        return (1);
    }
    fprintf(stderr, "DEBUG: Executing %s node: %p\n",
        get_token_str(node->type), (void *)node);
    if (node->type == TYPE_PIPE)
    {
        fprintf(stderr, "DEBUG: Executing pipe command\n");
        return (execute_pipeline(node, vars));
    }
    if (node->type == TYPE_OUT_REDIRECT || node->type == TYPE_APPEND_REDIRECT
        || node->type == TYPE_IN_REDIRECT || node->type == TYPE_HEREDOC)
    {
        return (exec_redirect_cmd(node, envp, vars));
    }
    return (exec_std_cmd(node, envp, vars));
}
