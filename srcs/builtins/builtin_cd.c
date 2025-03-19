/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:26 by lechan            #+#    #+#             */
/*   Updated: 2025/03/20 05:32:16 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: cd. Changes the current working directory.
- Master control function for cd command.
- Handles special directories (home and previous).
- Changes to the directory specified in args[1].
- Updates PWD and OLDPWD environment variables.
- Frees memory allocated for oldpwd.
Returns 0 on success, 1 on failure.
*/
int	builtin_cd(char **args, t_vars *vars)
{
	char	*oldpwd;
	int		cmdcode;

	oldpwd = ft_strdup(get_env_val("OLDPWD", vars->env));
	if (!oldpwd)
	{
		printf("cd: ft_strdup error\n");
		return (1);
	}
	cmdcode = handle_cd_path(args, vars);
	if (cmdcode != 0)
	{
		ft_safefree((void **)&oldpwd);
		return (cmdcode);
	}
	cmdcode = update_env_pwd(vars, oldpwd);
	if (cmdcode != 0)
	{
		ft_safefree((void **)&oldpwd);
		return (1);
	}
	ft_safefree((void **)&oldpwd);
	if (vars->pipeline != NULL)
		vars->pipeline->last_cmdcode = cmdcode;
	return (cmdcode);
}

/*
Handle changing to special directories (home or previous).
- Changes to HOME directory if no arguments or "~" is given.
- Changes to OLDPWD directory if "-" is given.
- Gets the path from the environment variables.
Returns 0 on success, 1 on failure.
*/
int	handle_cd_special(char **args, t_vars *vars)
{
	char	*path_value;
	int		cmdcode;
	
	if ((!args[1]) || ((args[1][0] == '~') && (args[1][1] == '\0')))
	{
		path_value = get_env_val("HOME", vars->env);
		cmdcode = chdir(path_value);
		if (cmdcode != 0)
		{
			printf("cd: HOME not set or no access\n");
			return (1);
		}
		return (0);
	}
	path_value = get_env_val("OLDPWD", vars->env);
	cmdcode = chdir(path_value);
	if (cmdcode != 0)
	{
		printf("cd: OLDPWD not set or no access\n");
		return (1);
	}
	printf("%s\n", path_value);
	return (0);
}

/*
Handle directory change based on arguments.
- Changes to the directory specified in args[1].
- Prints error message if directory does not exist.
Returns 0 on success, 1 on failure.
*/
int	handle_cd_path(char **args, t_vars *vars)
{
	int	cmdcode;
	
	if ((!args[1]) || ((args[1][0] == '~') && (args[1][1] == '\0')) ||
		(args[1][0] == '-' && args[1][1] == '\0'))
	{
		return (handle_cd_special(args, vars));
	}
	cmdcode = chdir(args[1]);
	if (cmdcode != 0)
	{
		printf("cd: no such file or directory: %s\n", args[1]);
		return (1);
	}
	return (0);
}

/*
Updates PWD and OLDPWD environment variables after directory change.
- Temporarily stores the old working directory in OLDPWD.
- Temporarily buffers the current working directory path in cwd[].
- Updates the PWD environment variable with the new path.
Returns 0 on success, 1 on failure.
OLD VERSION

int	update_env_pwd(t_vars *vars, char *oldpwd)
{
	char	cwd[1024];
	char	*tmp;
	int     cmdcode;
	
	tmp = ft_strjoin("OLDPWD=", oldpwd);
	if (!tmp)
		return (1);
	modify_env(&vars->env, 1, tmp);
	ft_safefree((void **)&tmp);
	cmdcode = getcwd(cwd, sizeof(cwd));
	if (!cmdcode)
	{
		printf("cd: error retrieving current directory\n");
		return (1);
	}
	tmp = ft_strjoin("PWD=", cwd);
	if (!tmp)
		return (1);
	modify_env(&vars->env, 1, tmp);
	ft_safefree((void **)&tmp);
	return (0);
}
*/
int update_env_pwd(t_vars *vars, char *oldpwd)
{
    char    cwd[1024];
    char    *tmp;
    char    *result;
    
    tmp = ft_strjoin("OLDPWD=", oldpwd);
    if (!tmp)
        return (1);
    modify_env(&vars->env, 1, tmp);
    ft_safefree((void **)&tmp);
    // Fix: getcwd returns a char* on success, NULL on failure
    result = getcwd(cwd, sizeof(cwd));
    if (result == NULL)
    {
        ft_putstr_fd("cd: error retrieving current directory\n", 2);
        return (1);
    }
    
    tmp = ft_strjoin("PWD=", cwd);
    if (!tmp)
        return (1);
    modify_env(&vars->env, 1, tmp);
    ft_safefree((void **)&tmp);
    return (0);
}
