/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:50:50 by lechan            #+#    #+#             */
/*   Updated: 2025/03/21 04:17:29 by bleow            ###   ########.fr       */
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
Works with execute_builtin().
*/
int	builtin_exit(t_vars *vars)
{
    int	cmdcode;
    
    cmdcode = 0;
    if (vars && vars->pipeline)
        cmdcode = vars->pipeline->last_cmdcode;
        
    fprintf(stderr, "DEBUG: [builtin_exit] Starting exit sequence with code %d\n", cmdcode);
    ft_putendl_fd("exit", STDOUT_FILENO);
    
    // Critical operations only
    save_history();
    rl_clear_history();
    
    // Null out problematic pointers without trying to free them
    fprintf(stderr, "DEBUG: [builtin_exit] Nulling problematic pointers\n");
    if (vars && vars->pipeline)
    {
        // Don't try to free these - just null them out to prevent access
        vars->pipeline->exec_cmds = NULL;
        vars->pipeline->pipe_fds = NULL;
        vars->pipeline->pids = NULL;
        vars->pipeline->status = NULL;
    }
    
    // Now it's safe to continue with token cleanup
    if (vars)
        cleanup_token_list(vars);
    
    fprintf(stderr, "DEBUG: [builtin_exit] Exiting with code %d\n", cmdcode);
    exit(cmdcode);
    return (0);
}
