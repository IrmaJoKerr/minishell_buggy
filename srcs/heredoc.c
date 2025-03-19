/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 05:39:02 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 18:42:58 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Joins a chunk to an existing string, freeing the original string.
- Takes base string and chunk to append.
- Creates new joined string and frees original.
Returns:
Newly allocated string with combined content.
Original input string if chunk is NULL.
New copy of chunk if base string is NULL.
Works with expand_one_line().

Example: str = "Hello", chunk = " World"
- Returns: "Hello World"
- Original "Hello" string is freed
*/
char	*merge_and_free(char *str, char *chunk)
{
    char	*new_str;

    if (!chunk)
        return (str);
    if (!str)
        return (ft_strdup(chunk));
    new_str = ft_strjoin(str, chunk);
    ft_safefree((void **)&str);
    return (new_str);
}

/*
Extracts and expands a variable from heredoc line.
- Processes a variable starting with $ character.
- Uses handle_expansion() to perform the expansion.
- Falls back to returning "$" if expansion fails.
Returns:
Newly allocated string with expanded variable.
"$" if variable can't be expanded.
Works with expand_one_line().

Example: At position of "$HOME" in a line
- Returns: "/Users/username" (expanded value)
- Updates position to after variable name
*/
char	*expand_heredoc_var(char *line, int *pos, t_vars *vars)
{
    char	*expanded;
    int		old_pos;

    old_pos = *pos;
    expanded = handle_expansion(line, pos, vars);
    if (!expanded)
    {
        *pos = old_pos + 1;
        return (ft_strdup("$"));
    }
    return (expanded);
}

/*
Extracts a regular text string up to $ character.
- Processes text segment from current position to next $.
- Creates substring of this text segment.
Returns:
Newly allocated substring from start to next variable.
NULL if no characters to extract.
Works with expand_one_line().

Example: At start of "Hello $USER"
- Returns: "Hello "
- Updates position to the $ character
*/
char	*read_heredoc_str(char *line, int *pos)
{
    int	start;

    start = *pos;
    while (line[*pos] && line[*pos] != '$')
        (*pos)++;
    if (*pos > start)
        return (ft_substr(line, start, *pos - start));
    return (NULL);
}

/*
Processes one segment of a heredoc line.
- Handles either variable expansion or regular text.
- Joins processed segment to result string.
- Updates position for next segment processing.
Returns:
Updated result string with new segment added.
Works with expand_heredoc_line().

Example: For input "Hello $HOME"
- First call processes "Hello " (regular text)
- Second call processes "$HOME" (variable)
- Returns combined result after each call
*/
char	*expand_one_line(char *line, int *pos, t_vars *vars, char *result)
{
    char	*segment;

    if (line[*pos] == '$')
    {
        segment = expand_heredoc_var(line, pos, vars);
        result = merge_and_free(result, segment);
        ft_safefree((void **)&segment);
    }
    else
    {
        segment = read_heredoc_str(line, pos);
        result = merge_and_free(result, segment);
        ft_safefree((void **)&segment);
    }
    return (result);
}

/*
Expands all variables in a heredoc line.
- Processes entire line character by character.
- Builds result by combining expanded segments.
- Handles all variable expansions in the line.
Returns:
New string with all variables expanded.
Empty string on NULL input or if no expansions.
Works with write_to_heredoc().

Example: Input "Hello $USER world"
- Expands to "Hello username world"
- All $VAR references replaced with their values
*/
char	*expand_heredoc_line(char *line, t_vars *vars)
{
    int		pos;
    char	*result;

    if (!line || !vars)
        return (ft_strdup(""));
    result = NULL;
    pos = 0;
    while (line[pos])
        result = expand_one_line(line, &pos, vars, result);
    if (!result)
        return (ft_strdup(""));
    return (result);
}

/*
Checks if a heredoc delimiter contains quotes.
- Examines delimiter character by character.
- Determines if variables should be expanded.
Returns:
1 if variables should be expanded (no quotes).
0 if variables shouldn't be expanded (has quotes).
Works with handle_heredoc().

Example: "EOF" -> 1 (expand variables)
         "'EOF'" -> 0 (don't expand variables)
*/
int	chk_expand_heredoc(char *delimiter)
{
    int	i;

    if (!delimiter)
        return (0);
    i = 0;
    while (delimiter[i])
    {
        if (ft_isquote(delimiter[i]))
            return (0);
        i++;
    }
    return (1);
}

/*
Writes line to heredoc pipe with variable expansion.
- Handles newline addition to each input line.
- Optionally expands variables based on delimiter quotes.
- Manages all write operations and error handling.
Returns:
1 on successful write.
0 on any failure.
Works with read_heredoc().

Example: Line "echo $HOME" with expand_vars=true
- Expands to "echo /Users/username"
- Writes expanded content plus newline to fd
*/
int	write_to_heredoc(int fd, char *line, t_vars *vars, int expand_vars)
{
    char	*expanded_line;
    int		write_result;
    int		newline_result;

    if (!line)
        return (0);
    if (expand_vars && vars)
    {
        expanded_line = expand_heredoc_line(line, vars);
        if (!expanded_line)
            return (0);
        write_result = write(fd, expanded_line, ft_strlen(expanded_line));
        newline_result = write(fd, "\n", 1);
        ft_safefree((void **)&expanded_line);
        if (write_result == -1 || newline_result == -1)
            return (0);
        return (1);
    }
    write_result = write(fd, line, ft_strlen(line));
    newline_result = write(fd, "\n", 1);
    if (write_result == -1 || newline_result == -1)
        return (0);
    return (1);
}

/*
Reads input for heredoc until delimiter is encountered.
- Prompts user for input lines with "> ".
- Compares each line against delimiter.
- Writes valid lines to the pipe.
Returns:
1 when completed successfully.
0 on any error.
Works with handle_heredoc().

Example: With delimiter "EOF"
- Reads lines like "Hello", "$USER", "EOF"
- Writes "Hello" and expanded "$USER" to pipe
- Stops at "EOF" line, returning 1
*/
int	read_heredoc(int *fd, char *delimiter, t_vars *vars, int expand_vars)
{
    char	*line;
    int		write_success;

    while (1)
    {
        line = readline("> ");
        if (!line)
            break ;
        if (ft_strcmp(line, delimiter) == 0)
        {
            ft_safefree((void **)&line);
            break ;
        }
        write_success = write_to_heredoc(fd[1], line, vars, expand_vars);
        ft_safefree((void **)&line);
        if (!write_success)
            return (0);
    }
    return (1);
}

/*
Creates pipe to handle heredoc redirection error cases.
- Validates node has required arguments.
- Sets error code appropriately.
Returns:
-1 to indicate error condition.
Works with handle_heredoc().
*/
int	handle_heredoc_err(t_node *node, t_vars *vars)
{
    if (!node || !node->args || !node->args[0])
    {
        vars->error_code = 1;
        return (-1);
    }
    vars->error_code = 1;
    return (-1);
}

/*
Cleans up resources after heredoc failure.
- Closes pipe file descriptors.
- Sets error code.
Returns:
-1 to indicate error condition.
Works with handle_heredoc().
*/
int	cleanup_heredoc_fail(int *fd, t_vars *vars)
{
    close(fd[0]);
    close(fd[1]);
    vars->error_code = 1;
    return (-1);
}

/*
Creates a pipe for heredoc redirection.
- Creates pipe file descriptors.
- Determines if variables should be expanded.
- Reads input until delimiter or EOF.
Returns:
File descriptor for reading heredoc content.
-1 on any error.
Works with proc_heredoc().

Example: Node with delimiter "EOF"
- Creates pipe
- Reads input lines until "EOF"
- Returns read end of pipe for command input
*/
int	handle_heredoc(t_node *node, t_vars *vars)
{
    int	fd[2];
    int	expand_vars;
    int	read_success;

    if (!node || !node->args || !node->args[0])
        return (handle_heredoc_err(node, vars));
    if (pipe(fd) == -1)
        return (handle_heredoc_err(node, vars));
    expand_vars = chk_expand_heredoc(node->args[0]);
    read_success = read_heredoc(fd, node->args[0], vars, expand_vars);
    if (!read_success)
        return (cleanup_heredoc_fail(fd, vars));
    close(fd[1]);
    return (fd[0]);
}

/*
Main function for heredoc redirection process.
- Gets file descriptor with heredoc content.
- Redirects stdin to read from this descriptor.
- Ensures proper cleanup of file descriptors.
Returns:
1 on successful redirection setup.
0 on any error.
Works with setup_redirection().

Example: For shell command "cat << EOF"
- Input: "cat << EOF" 
- Shell prompts for input with "> "
- User enters multiple lines: "Hello", "World", "EOF"
- handle_heredoc() captures "Hello" and "World" into a pipe
- proc_heredoc() redirects stdin to read from this pipe
- When "cat" executes, it reads "Hello\nWorld\n" from stdin
- Output: "Hello" and "World" appear on screen
This creates the effect of an inline document for commands
that read from stdin.
*/
int	proc_heredoc(t_node *node, t_vars *vars)
{
    int	fd;

    fd = handle_heredoc(node, vars);
    if (fd == -1)
        return (0);
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        close(fd);
        vars->error_code = 1;
        return (0);
    }
    close(fd);
    return (1);
}
