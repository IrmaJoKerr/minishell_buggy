/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:35 by lechan            #+#    #+#             */
/*   Updated: 2025/03/18 11:40:04 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: echo. Writes arguments to standard output.
-  Newline flag is set to 1 if the -n flag is not present and 0 if it is
   present.
   (Normally a newline is printed at the end of the echo command)
Returns command status (0 for success, 1 for failure).
*/
int	builtin_echo(char **args, t_vars *vars)
{
	int	i;
	int	newline;
	int	cmdcode;

	cmdcode = 0;
	if (!args || !args[0])
	{
		cmdcode = 1;
		if (vars->pipeline != NULL)
			vars->pipeline->last_cmdcode = cmdcode;
		return (cmdcode);
	}
	i = 1;
	newline = 1;
	if (args[i] && ft_strcmp(args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	process_echo_args(args, i, newline);
	if (vars->pipeline != NULL)
		vars->pipeline->last_cmdcode = cmdcode;
	return (cmdcode);
}

/*
Process and print echo command arguments.
Adds a space after each argument if there is more than one argument.
Adds newline if needed.
Returns 0 on success.
*/
int process_echo_args(char **args, int start, int nl_flag)
{
	int i;
	int j;
	
	i = start;
	while (args[i])
	{
		j = 0;
		while (args[i][j])
		{
			write(1, &args[i][j], 1);
			j++;
		}
		if (args[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (nl_flag)
		write(1, "\n", 1);
	return (0);
}
