/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:50 by lechan            #+#    #+#             */
/*   Updated: 2025/03/16 01:47:50 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <readline/readline.h>

/*
Built-in command: exit. Exits the shell.
- Initializes cmdcode to 0 then updates it with the last command code.
- Prints "exit" to STDOUT.
- Saves history to HISTORY_FILE.
- Clears readline history.
- Calls cleanup_exit() to free all allocated memory.
- Exits the program with the last command code.
*/
int	builtin_exit(t_vars *vars)
{
	int	cmdcode;
	
	cmdcode = 0;
	if (vars && vars->pipeline)
        cmdcode = vars->pipeline->last_cmdcode;
	ft_putendl_fd("exit", STDOUT_FILENO);
	save_history();
	rl_clear_history();
	cleanup_exit(vars);
	exit(cmdcode);
	return (0);
}
