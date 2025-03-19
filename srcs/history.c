/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 06:29:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 23:39:40 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Opens history file with appropriate access mode.
- Checks if history file exists and has required permissions.
- Creates new history file with proper permissions if needed.
- Opens file with requested mode (read or write).
Returns:
File descriptor for history file or -1 on error.
Works with load_history() and save_history().

Example: init_history_fd(O_RDONLY)
- Checks if HISTORY_FILE exists and is readable
- Opens it for reading if exists and accessible
- Returns file descriptor or -1 on failure
*/
int	init_history_fd(int mode)
{
    int	fd;

    if (access(HISTORY_FILE, F_OK) == -1)
    {
        fd = open(HISTORY_FILE, O_WRONLY | O_CREAT, 0644);
        if (fd == -1)
            return (-1);
        close(fd);
    }
    if (mode == O_RDONLY && access(HISTORY_FILE, R_OK) == -1)
        return (-1);
    if (mode == O_WRONLY && access(HISTORY_FILE, W_OK) == -1)
        return (-1);
    fd = open(HISTORY_FILE, mode);
    return (fd);
}

/*
Appends command line to both history file and memory.
- Writes provided line to the history file with newline.
- Adds line to readline's history for in-memory access.
Returns:
1 on success, 0 on failure.
Works with save_history().

Example: append_history(fd, "ls -la")
- Writes "ls -la\n" to history file
- Adds "ls -la" to readline's history
- Returns 1 if successful
*/
int	append_history(int fd, const char *line)
{
    ssize_t	write_ret;

    if (!line)
        return (0);
    write_ret = write(fd, line, ft_strlen(line));
    if (write_ret == -1)
        return (0);
    write_ret = write(fd, "\n", 1);
    if (write_ret == -1)
        return (0);
    add_history(line);
    return (1);
}

/*
Counts the number of lines in the history file.
- Opens history file for reading.
- Reads each line using get_next_line() until EOF.
- Counts each line and properly frees memory.
Returns:
Total count of history entries in the file.
0 if file cannot be opened or is empty.
Works with load_history() and trim_history().

Example: If history file contains 100 commands
- Returns 100 after counting all lines
- Returns 0 if file doesn't exist or can't be opened
*/
int	get_history_count(void)
{
    int		fd;
    int		count;
    char	*line;

    count = 0;
    fd = init_history_fd(O_RDONLY);
    if (fd == -1)
        return (0);
    line = get_next_line(fd);
    while (line)
    {
        count++;
        ft_safefree((void **)&line);
        line = get_next_line(fd);
    }
    close(fd);
    return (count);
}
