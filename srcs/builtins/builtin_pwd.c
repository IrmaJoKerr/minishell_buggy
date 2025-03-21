/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:51:01 by lechan            #+#    #+#             */
/*   Updated: 2025/03/20 05:32:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Built-in command: pwd. Prints the current working directory.
- Allocates memory for the current working directory string.
- Gets the current working directory from the PWD environment variable.
- Prints the current working directory to STDOUT.
- Frees the current working directory string.
Returns 0 on success, 1 on failure.
*/
int	builtin_pwd(t_vars *vars)
{
    char	*cwd;
    int		cmdcode;

    cmdcode = 0;
    if (!vars || !vars->env)
    {
        cmdcode = 1;
        if (vars && vars->pipeline != NULL)
            vars->pipeline->last_cmdcode = cmdcode;
        return (cmdcode);
    }
    cwd = get_env_val("PWD", vars->env);
    if (!cwd || !*cwd)
    {
        ft_safefree((void **)&cwd);
        cmdcode = 1;
        if (vars->pipeline != NULL)
            vars->pipeline->last_cmdcode = cmdcode;
        return (cmdcode);
    }
    printf("%s\n", cwd);
    ft_safefree((void **)&cwd);
    if (vars->pipeline != NULL)
        vars->pipeline->last_cmdcode = cmdcode;
    return (cmdcode);
}
