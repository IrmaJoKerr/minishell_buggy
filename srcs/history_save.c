/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_save.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 14:35:22 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 23:39:48 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Helper function to perform actual file copy operation.
- Reads from source file in chunks of 4096 bytes.
- Writes each chunk to destination file.
- Continues until entire file is copied.
Returns:
1 on successful copy, 0 on any read/write error.
Works with copy_file().

Example: For a 10KB history file
- Reads file in ~3 chunks of 4096 bytes
- Writes each chunk to destination
- Returns 1 when complete copy is successful
*/
int	copy_file_content(int fd_src, int fd_dst)
{
    char	buffer[4096];
    ssize_t	bytes;

    bytes = read(fd_src, buffer, 4096);
    while (bytes > 0)
    {
        if (write(fd_dst, buffer, bytes) == -1)
            return (0);
        bytes = read(fd_src, buffer, 4096);
    }
    return (bytes >= 0);
}

/*
Copies file contents from source to destination.
- Opens source file for reading.
- Opens destination file for writing (creates if needed).
- Calls copy_file_content to perform the actual copy.
- Ensures proper cleanup of file descriptors.
Returns:
1 on successful copy, 0 on any error.
Works with trim_history().

Example: copy_file(HISTORY_FILE_TMP, HISTORY_FILE)
- Copies from temporary history file to main history file
- Creates or overwrites destination file with source content
- Returns 1 on successful operation
*/
int	copy_file(const char *src, const char *dst)
{
    int		fd_src;
    int		fd_dst;
    int		result;

    fd_src = open(src, O_RDONLY);
    if (fd_src == -1)
        return (0);
    fd_dst = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst == -1)
    {
        close(fd_src);
        return (0);
    }
    result = copy_file_content(fd_src, fd_dst);
    close(fd_src);
    close(fd_dst);
    return (result);
}

/*
Copies lines to temporary history file for safe operations.
- Reads lines one by one from source file descriptor.
- Writes each line to temporary file with newline.
- Provides safety during history file manipulation.
Returns:
1 on successful copy, 0 on any error.
Works with trim_history().

Example: During history trimming
- Copies newer history entries to temporary file
- Protects against data corruption during manipulation
- Returns 1 when temporary file is ready for use
*/
int	copy_to_temp(int fd_read)
{
    int		fd_write;
    char	*line;

    fd_write = open(HISTORY_FILE_TMP, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_write == -1)
        return (0);
    line = get_next_line(fd_read);
    while (line)
    {
        write(fd_write, line, ft_strlen(line));
        ft_safefree((void **)&line);
        line = get_next_line(fd_read);
    }
    close(fd_write);
    return (1);
}

/*
Skips specified number of lines in an open file.
- Reads and discards requested number of lines.
- Uses get_next_line to properly handle line endings.
- Ensures proper memory management during skipping.
Returns:
Nothing (void function).
Works with trim_history().

Example: For a history with 1000 lines
- skip_lines(fd, 500) skips first 500 entries
- Positions file pointer at 501st entry
- Properly frees all memory used during skipping
*/
void	skip_lines(int fd, int count)
{
    char	*line;
    int		i;

    i = 0;
    while (i < count)
    {
        line = get_next_line(fd);
        if (!line)
            break ;
        ft_safefree((void **)&line);
        i++;
    }
}

/*
Trims history file to maximum allowed size.
- Opens history file for reading.
- Skips oldest entries if file exceeds size limit.
- Copies remaining entries to temporary file.
- Replaces original file with trimmed version.
- Removes temporary file after successful operation.
Returns:
Nothing (void function).
Works with save_history().

Example: If HISTORY_FILE_MAX=1000 and file has 1200 entries
- Skips first 200 entries (oldest commands)
- Copies newest 1000 entries to temporary file
- Replaces original history file with trimmed version
- Maintains history file within configured size limit
*/
void	trim_history(int excess_lines)
{
    int	fd;

    fd = init_history_fd(O_RDONLY);
    if (fd == -1)
        return ;
    skip_lines(fd, excess_lines);
    if (!copy_to_temp(fd))
    {
        close(fd);
        return ;
    }
    close(fd);
    if (copy_file(HISTORY_FILE_TMP, HISTORY_FILE))
        unlink(HISTORY_FILE_TMP);
}

/*
Saves readline history entries to history file.
- Opens history file for writing.
- Retrieves history entries from readline's memory.
- Skips excess entries if count exceeds HISTORY_FILE_MAX.
- Writes valid entries to history file with newlines.
- Logs details of the save operation for debugging.
Returns:
Nothing (void function).
Works with cleanup_exit() during shell termination.

Example: When shell exits with 1500 history entries and HISTORY_FILE_MAX=1000
- Opens history file for writing (creating if needed)
- Calculates excess entries (500)
- Skips oldest 500 entries
- Writes newest 1000 entries to history file
- Logs success with number of entries saved
*/
void	save_history(void)
{
    int			fd;
    HIST_ENTRY	**hist_list;
    int			i;
    int			history_count;
    int			excess_lines;

    fd = open(HISTORY_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("bleshell: error opening history file");
        return ;
    }
    hist_list = history_list();
    if (!hist_list)
    {
        close(fd);
        return ;
    }
    history_count = history_length;
    excess_lines = history_count - HISTORY_FILE_MAX;
    i = (excess_lines > 0) ? excess_lines : 0;
    save_history_entries(fd, hist_list, i, history_count);
    close(fd);
}

/*
Writes history entries to the history file.
- Iterates through history entries starting at specified index.
- Writes valid entries to file with newlines.
- Tracks number of entries saved for debugging.
- Logs detailed save information for troubleshooting.
Returns:
Number of entries successfully saved.
Works with save_history().

Example: For 1000 history entries after skipping 200
- Iterates through entries 200-999
- Writes each valid entry with a newline
- Logs progress during save operation
- Returns total count of entries saved (should be 800)
*/
int	save_history_entries(int fd, HIST_ENTRY **hist_list, int start, int total)
{
    int	i;
    int	saved_count;

    saved_count = 0;
    fprintf(stderr, "DEBUG: Saving history with %d entries total\n", 
        total - start);
    i = start;
    while (i < total && hist_list[i])
    {
        if (hist_list[i]->line && *(hist_list[i]->line))
        {
            saved_count++;
            write(fd, hist_list[i]->line, strlen(hist_list[i]->line));
            write(fd, "\n", 1);
            fprintf(stderr, "DEBUG: Saved line #%d of %d: '%s'\n",
                saved_count, total - start, hist_list[i]->line);
        }
        i++;
    }
    fprintf(stderr, "DEBUG: Successfully saved %d history entries\n", 
        saved_count);
    return (saved_count);
}
