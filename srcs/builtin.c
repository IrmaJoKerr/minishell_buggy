/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:33:49 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 18:01:10 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Checks if a command is a shell builtin.
- Tests command name against all builtin commands.
- Shell builtins: echo, cd, pwd, export, unset, env, exit.
Returns:
1 if command is a builtin.
0 if command is not a builtin or is NULL.
Works with execute_builtin().
*/
int	is_builtin(char *cmd)
{
    if (!cmd)
        return (0);
    if (!ft_strcmp(cmd, "cd"))
        return (1);
    if (!ft_strcmp(cmd, "echo"))
        return (1);
    if (!ft_strcmp(cmd, "env"))
        return (1);
    if (!ft_strcmp(cmd, "exit"))
        return (1);
    if (!ft_strcmp(cmd, "export"))
        return (1);
    if (!ft_strcmp(cmd, "pwd"))
        return (1);
    if (!ft_strcmp(cmd, "unset"))
        return (1);
    return (0);
}

/*
Executes the appropriate builtin command function.
- Identifies which builtin to call based on command name.
- Passes arguments and environment to the specific builtin.
- Each builtin handles its own error messages and reporting.
Returns:
The exit status from the executed builtin.
1 if command is invalid (should never happen).
Works with handle_builtin_cmd().

Example: For "cd /home"
- Calls builtin_cd() with args={"cd", "/home"} and vars
- Returns status code (0 for success, non-zero for error)
*/
int	execute_builtin(char *cmd, char **args, t_vars *vars)
{
    fprintf(stderr, "DEBUG: execute_builtin called with cmd: %s\n", cmd);
    
    if (!ft_strcmp(cmd, "cd"))
        return (builtin_cd(args, vars));
    if (!ft_strcmp(cmd, "echo"))
        return (builtin_echo(args, vars));
    if (!ft_strcmp(cmd, "env"))
        return (builtin_env(vars));
    if (!ft_strcmp(cmd, "exit"))
        return (builtin_exit(vars));
    if (!ft_strcmp(cmd, "export"))
        return (builtin_export(args, vars));
    if (!ft_strcmp(cmd, "pwd"))
        return (builtin_pwd(vars));
    if (!ft_strcmp(cmd, "unset"))
        return (builtin_unset(args, vars));
    return (1);
}
