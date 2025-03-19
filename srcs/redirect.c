/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:51:05 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 22:45:17 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Checks file permissions before redirection starts.
- For read mode: Verifies file exists and is readable.
- For write mode: Checks if file exists and is writable.
- For new files: Returns 1 to allow creation.
Returns:
- 1 if file can be accessed with requested mode.
- 0 if not.
Works with open_redirect_file() for permission validation.

Example: When redirecting output to file
- Checks if file exists and can be written to
- Returns 1 if accessible, 0 if permission denied
*/
int	chk_permissions(char *filename, int mode, t_vars *vars)
{
    if (!filename)
        return (0);
    if (mode == O_RDONLY && access(filename, R_OK) == -1)
        return (redirect_error(filename, vars, 1));
    if ((mode & O_WRONLY) && access(filename, W_OK) == -1)
    {
        if (access(filename, F_OK) == -1)
            return (1);
        return (redirect_error(filename, vars, 1));
    }
    return (1);
}

/*
Sets file open flags for output redirection.
- Configures flags for > (overwrite) or >> (append) modes.
- Always includes O_WRONLY and O_CREAT flags.
- Adds O_APPEND for append mode or O_TRUNC for overwrite.
Returns:
- Combined flag value to use with open().
Works with output_redirect() to set proper file flags.

Example: For append redirection ">>"
- Returns O_WRONLY | O_CREAT | O_APPEND
- For standard redirection, returns O_WRONLY | O_CREAT | O_TRUNC
*/
int	set_output_flags(int append)
{
    int	flags;

    flags = O_WRONLY | O_CREAT;
    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    return (flags);
}

/*
Sets file open flags for all redirection types.
- For input (<): Uses O_RDONLY.
- For output (>): Uses O_WRONLY | O_CREAT | O_TRUNC.
- For append (>>): Uses O_WRONLY | O_CREAT | O_APPEND.
Returns:
- Combined flag value to use with open().
Works with open_redirect_file() to set file access mode.

Example: For different redirection types
- mode 0 (input): Returns O_RDONLY
- mode 1 (output): Returns O_WRONLY | O_CREAT | O_TRUNC
- mode 2 (append): Returns O_WRONLY | O_CREAT | O_APPEND
*/
int	set_redirect_flags(int mode)
{
    int	flags;

    if (mode == 0)
        flags = O_RDONLY;
    else
    {
        flags = O_WRONLY | O_CREAT;
        if (mode == 2)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;
    }
    return (flags);
}

/*
Determines if token is a redirection operator.
- Checks if token type matches any redirection types.
- Includes all input and output redirection variants.
Returns:
- 1 if token is a redirection.
- 0 if not.
Works with process_redirections() and other redirection handlers.

Example: When processing token list
- Returns 1 for tokens of type <, >, >>, or <<
- Returns 0 for command, pipe, or other token types
*/
int	is_redirection(t_tokentype type)
{
    return (type == TYPE_HEREDOC
        || type == TYPE_IN_REDIRECT
        || type == TYPE_OUT_REDIRECT
        || type == TYPE_APPEND_REDIRECT);
}

/*
Resets saved standard file descriptors.
- Restores original stdin and stdout if they were changed.
- Closes any open heredoc file descriptor.
- Updates the pipeline state in vars.
Works with execute_cmd() to clean up after command execution.

Example: After command execution
- Restores original stdin/stdout
- Cleans up any open file descriptors
- Resets pipeline state for next command
*/
void	reset_redirect_fds(t_vars *vars)
{
    if (!vars || !vars->pipeline)
        return ;
    if (vars->pipeline->saved_stdin > 2)
    {
        dup2(vars->pipeline->saved_stdin, STDIN_FILENO);
        close(vars->pipeline->saved_stdin);
        vars->pipeline->saved_stdin = -1;
    }
    if (vars->pipeline->saved_stdout > 2)
    {
        dup2(vars->pipeline->saved_stdout, STDOUT_FILENO);
        close(vars->pipeline->saved_stdout);
        vars->pipeline->saved_stdout = -1;
    }
    if (vars->pipeline->heredoc_fd > 2)
    {
        close(vars->pipeline->heredoc_fd);
        vars->pipeline->heredoc_fd = -1;
    }
}

/*
Handles input redirection (< filename).
- Validates the redirection node and args.
- Checks read permissions on target file.
- Opens file in read-only mode.
- Sets fd_in to the opened file descriptor.
Returns:
- 1 on success.
- 0 on failure (invalid args, permissions, open error).
Works with handle_redirect_cmd() for input setup.

Example: For "cmd < input.txt"
- Opens "input.txt" for reading
- Sets fd_in to the file descriptor
- Returns success/failure
*/
int	input_redirect(t_node *node, int *fd_in, t_vars *vars)
{
    if (!node || !node->args || !node->args[0] || !fd_in)
        return (0);
    if (!chk_permissions(node->args[0], O_RDONLY, vars))
        return (0);
    *fd_in = open(node->args[0], O_RDONLY);
    if (*fd_in == -1)
        return (redirect_error(node->args[0], vars, 1));
    return (1);
}

/*
Handles output redirection (> or >> filename).
- Validates the redirection node and args.
- Sets flags based on append mode.
- Checks write permissions on target file.
- Opens file with appropriate flags and permissions.
Returns:
- 1 on success.
- 0 on failure (invalid args, permissions, open error).
Works with handle_redirect_cmd() for output setup.

Example: For "cmd > output.txt"
- Opens "output.txt" for writing with truncate flag
- Sets fd_out to the file descriptor
- Returns success/failure
*/
int	output_redirect(t_node *node, int *fd_out, int append, t_vars *vars)
{
    int	flags;

    if (!node || !node->args || !node->args[0] || !fd_out)
        return (0);
    flags = set_output_flags(append);
    if (!chk_permissions(node->args[0], flags, vars))
        return (0);
    *fd_out = open(node->args[0], flags, 0644);
    if (*fd_out == -1)
        return (redirect_error(node->args[0], vars, 1));
    return (1);
}

/*
Opens file for redirection with appropriate mode.
- Sets flags based on redirection type.
- Validates file permissions.
- Opens file with correct flags.
- Reports errors if file operations fail.
Returns:
- 1 on success.
- 0 on failure.
Works with handle_redirect() for file opening.

Example: Opening a file for redirection
- Sets correct flags based on redirection type
- Verifies permissions
- Opens file and captures descriptor
- Reports errors if necessary
*/
int	open_redirect_file(t_node *node, int *fd, int mode, t_vars *vars)
{
    int	flags;

    if (!node || !node->args || !node->args[0] || !fd)
        return (0);
    flags = set_redirect_flags(mode);
    if (!chk_permissions(node->args[0], flags, vars))
        return (0);
    if (mode == 0)
        *fd = open(node->args[0], flags);
    else
        *fd = open(node->args[0], flags, 0644);
    if (*fd == -1)
        return (redirect_error(node->args[0], vars, 1));
    return (1);
}

/*
Main redirection controller function.
- Opens redirection file.
- Determines which standard descriptor to redirect.
- Redirects standard descriptor to opened file.
- Cleans up original descriptor to prevent leaks.
Returns:
- 1 on success.
- 0 on any failure.
Works with execute_redirects() for I/O redirection.

Example: For "cmd > output.txt"
- Opens output.txt
- Redirects stdout to this file
- Closes original file descriptor
- Returns success status
*/
int	handle_redirect(t_node *node, int *fd, int mode, t_vars *vars)
{
    int	std_fd;
    int	result;
    int	success;

    success = open_redirect_file(node, fd, mode, vars);
    if (!success)
        return (0);
    if (mode == 0)
        std_fd = STDIN_FILENO;
    else
        std_fd = STDOUT_FILENO;
    result = dup2(*fd, std_fd);
    if (result == -1)
    {
        close(*fd);
        return (0);
    }
    close(*fd);
    return (1);
}
