/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paths.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:23:30 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 23:39:14 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Extracts PATH directories from environment variables.
- Searches for the variable starting with "PATH=".
- Splits the PATH value by colon delimiter.
- Handles NULL environment array or missing PATH.
Returns:
- Array of path strings.
- NULL if PATH not found.
Works with search_in_env() for command path building.

Example: When PATH=/usr/bin:/bin:/usr/local/bin
- Returns array with ["/usr/bin", "/bin", "/usr/local/bin", NULL]
- Returns NULL if PATH is not in environment
*/
char	**get_path_env(char **envp)
{
    int	i;

    i = 0;
    if (!envp)
        return (NULL);
    while (envp[i] && !ft_strnstr(envp[i], "PATH=", 5))
        i++;
    if (!envp[i])
        return (NULL);
    return (ft_split(envp[i] + 5, ':'));
}

/*
Checks if a command exists in a specific directory.
- Joins directory path with command name.
- Uses access() to verify file existence.
- Frees intermediate memory allocations.
Returns:
- Full path to executable if found.
- NULL if not found.
Works with search_in_env() during command search.

Example: try_path("/usr/bin", "ls")
- Checks if "/usr/bin/ls" exists and is accessible
- Returns "/usr/bin/ls" if found
- Returns NULL if file doesn't exist or can't be accessed
*/
char	*try_path(char *path, char *cmd)
{
    char	*part_path;
    char	*full_path;

    part_path = ft_strjoin(path, "/");
    full_path = ft_strjoin(part_path, cmd);
    ft_safefree((void **)&part_path);
    if (access(full_path, F_OK) == 0)
        return (full_path);
    ft_safefree((void **)&full_path);
    return (NULL);
}

/*
Searches for a command in PATH environment directories.
- Gets list of directories from PATH environment variable.
- Tries each directory to find the command.
- Reports detailed search progress for debugging.
Returns:
-Full path to command if found, NULL otherwise.
Works with get_cmd_path() for command resolution.

Example: For command "grep" with PATH=/usr/bin:/bin
- Searches in /usr/bin and /bin
- Returns "/usr/bin/grep" if found there
- Returns NULL if not found in any directory
*/
char	*search_in_env(char *cmd, char **envp)
{
    char	**paths;
    char	*path;
    int		i;

    paths = get_path_env(envp);
    if (!paths)
    {
        ft_putendl_fd("No PATH found in environment", 2);
        return (NULL);
    }
    i = 0;
    while (paths[i])
    {
        path = try_path(paths[i], cmd);
        if (path)
        {
            ft_free_2d(paths, ft_arrlen(paths));
            return (path);
        }
        i++;
    }
    ft_putstr_fd("Command not found in any PATH directory: ", 2);
    ft_putendl_fd(cmd, 2);
    ft_free_2d(paths, ft_arrlen(paths));
    return (NULL);
}

/*
Resolves command path for execution.
- Handles absolute and relative paths directly.
- Searches PATH environment for other commands.
- Verifies executable permissions.
Returns:
Full path to executable or NULL if not found/accessible.
Works with execute_cmd() during command execution.

Example: For "ls" command
- Checks if it's a direct path (starts with / or ./)
- If not, searches in PATH environment directories
- Returns "/bin/ls" (or similar) if found
- Returns NULL with error message if not found
*/
char	*get_cmd_path(char *cmd, char **envp)
{
    if (cmd[0] == '/' || (cmd[0] == '.' && (cmd[1] == '/'
        || (cmd[1] == '.' && cmd[2] == '/'))))
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        ft_putstr_fd("Direct path access denied: ", 2);
        ft_putendl_fd(cmd, 2);
        return (NULL);
    }
    return (search_in_env(cmd, envp));
}

/*
Creates a deep copy of environment variables array.
- Allocates memory for the new array.
- Duplicates each environment string.
- Handles memory allocation failures.
Returns:
Newly allocated copy of environment or NULL on failure.
Works with init_shell() and builtin environment commands.

Example: When executing a command with custom environment
- Duplicates all environment variables safely
- Provides independent environment array for modification
- Ensures memory safety with proper cleanup on errors
*/
char	**dup_env(char **envp)
{
    char	**env;
    size_t	env_size;
    size_t	i;

    env_size = ft_arrlen(envp);
    env = (char **)malloc(sizeof(char *) * (env_size + 1));
    if (!env)
        return (NULL);
    i = 0;
    while (i < env_size)
    {
        env[i] = ft_strdup(envp[i]);
        if (!env[i])
        {
            ft_free_2d(env, i);
            return (NULL);
        }
        i++;
    }
    env[env_size] = NULL;
    return (env);
}
