/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:56 by lechan            #+#    #+#             */
/*   Updated: 2025/03/18 11:39:46 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: export. 
- Sets or displays environment variables depending on arguments.
Example with arguments: export VAR_NAME="VALUE"
Example without arguments: export
Returns 0 on success, 1 on failure.
*/
int builtin_export(char **args, t_vars *vars)
{
	int cmdcode;
	
	if (!vars || !vars->env)
	{
		cmdcode = 1;
		if (vars && vars->pipeline)
			vars->pipeline->last_cmdcode = cmdcode;
		return (cmdcode);
	}
	if (!args[1])
		return (export_without_args(vars));
	return (export_with_args(args, vars));
}

/*
Handle export WITH NO ARGUMENTS.
- Prints all environment variables.
Returns 0 on success, 1 on failure.
*/
int	export_without_args(t_vars *vars)
{
	int	count;
	int	cmdcode;
	
	count = 0;
	while (vars->env[count])
		count++;
	cmdcode = sort_env(count, vars);
	if (vars->pipeline != NULL)
		vars->pipeline->last_cmdcode = cmdcode;
	return (cmdcode);
}

/*
Handle export WITH ARGUMENTS.
- Checks if arguments are valid.
- Sets environment variables.
Returns 0 on success, 1 on failure.
*/
int	export_with_args(char **args, t_vars *vars)
{
	int i;
	int cmdcode;
	
	i = 1;
	cmdcode = 0;
	while (args[i])
	{
		if (valid_export(args[i]))
			modify_env(&vars->env, 1, args[i]);
		else
		{
			printf("export: '%s': not a valid identifier\n", args[i]);
			cmdcode = 1;
		}
		i++;
	}
	if (vars->pipeline != NULL)
		vars->pipeline->last_cmdcode = cmdcode;
	return (cmdcode);
}

/*
Master control function that sorts and prints environment variables.
- Processes each environment variable and prints them in export format.
- Frees all allocated memory.
Returns 0 on success, 1 on failure.
*/
int sort_env(int count, t_vars *vars)
{
	int		i;
	int		cmdcode;
	char	**sort_env;
	
	sort_env = make_sorted_env(count, vars);
	if (!sort_env)
		return (1);
	i = 0;
	cmdcode = 0;
	while (sort_env[i])
	{
		cmdcode = process_export_var(sort_env[i]);
		i++;
	}
	i = 0;
	while (i < count)
	{
		ft_safefree((void **)&sort_env[i]);
		i++;
	}
	ft_safefree((void **)&sort_env);
	return (cmdcode);
}
