/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_level.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 14:19:12 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 06:06:52 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Gets the shell level from environment variables.
If SHLVL doesn't exist, creates it with value "1".
Stores the current SHLVL value in vars->shell_level.
Return:
- 0 on success.
- 1 on failure.
*/
int get_shell_level(t_vars *vars)
{
	int     i;
	char    *shlvl_str;
	char    *new_shlvl_str;
	 
	if (!vars || !vars->env)
		return (1);
	i = 0;
	while (vars->env[i])
	{
		if (ft_strncmp(vars->env[i], "SHLVL=", 6) == 0)
		{
			shlvl_str = vars->env[i] + 6;
			vars->shell_level = ft_atoi(shlvl_str);
			return (0);
		}
		i++;
	}
	new_shlvl_str = ft_strdup("SHLVL=1");
	if (!new_shlvl_str)
		return (1);
	vars->env[i] = new_shlvl_str;
	vars->env[i+1] = NULL;
	vars->shell_level = 1;
	return (0);
}
 
/*
Updates the SHLVL environment variable with the new value.
Return:
- 0 on success.
- 1 on failure.
*/
int update_shlvl_env(char **env, int position, int new_level)
{
	char *new_shlvl;
	char *new_env_entry;
	
	new_shlvl = ft_itoa(new_level);
	if (!new_shlvl)
		return (1);
	new_env_entry = ft_strjoin("SHLVL=", new_shlvl);
	ft_safefree((void **)&new_shlvl);
	if (!new_env_entry)
		return (1);
	ft_safefree((void **)&env[position]);
	env[position] = new_env_entry;
	return (0);
}

/*
Increments the shell level environment variable (SHLVL) by 1.
Updates the value in the environment and in vars->shell_level.
Return:
- 0 on success.
- 1 on failure.
*/
/*
Increments the shell level environment variable (SHLVL) by 1.
Updates the value in the environment and in vars->shell_level.
Return:
- 0 on success.
- 1 on failure.
*/
int incr_shell_level(t_vars *vars)
{
    int i;
    int result;
 
    fprintf(stderr, "DEBUG: Entering incr_shell_level with vars=%p\n", (void*)vars);
    if (!vars || !vars->env)
    {
        fprintf(stderr, "DEBUG: vars or vars->env is NULL\n");
        return (1);
    }
    
    vars->shell_level++;
    fprintf(stderr, "DEBUG: Incremented shell_level to %d\n", vars->shell_level);
    
    i = 0;
    while (vars->env[i])
    {
        fprintf(stderr, "DEBUG: Checking env entry %d: %s\n", i, vars->env[i]);
        if (ft_strncmp(vars->env[i], "SHLVL=", 6) == 0)
        {
            fprintf(stderr, "DEBUG: Found SHLVL at position %d\n", i);
            result = update_shlvl_env(vars->env, i, vars->shell_level);
            if (result != 0)
            {
                fprintf(stderr, "DEBUG: Failed to update SHLVL, error code: %d\n", result);
                return (1);
            }
            fprintf(stderr, "DEBUG: Successfully updated SHLVL. New value: %s\n", vars->env[i]);
            return (0);
        }
        i++;
    }
    
    fprintf(stderr, "DEBUG: SHLVL not found in environment, total env entries: %d\n", i);
    return (1);
}
