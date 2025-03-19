/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 23:01:47 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 23:40:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Retrieves the exit status from the pipeline or vars.
- Checks if pipeline exists in vars structure.
- Returns the last command code as a string.
- Falls back to "0" if information not available.
Returns:
Dynamically allocated string containing the exit status.
Works with handle_special_var().
*/
char	*chk_exitstatus(t_vars *vars)
{
    if (vars && vars->pipeline)
        return (ft_itoa(vars->pipeline->last_cmdcode));
    else
        return (ft_strdup("0"));
}

/*
Processes special shell variables like $? and $0.
- Handles $?: Returns exit status of last command.
- Handles $0: Returns shell name ("bleshell").
- For empty var_name: Returns an empty string.
Returns:
Newly allocated string with variable value.
NULL if not a special variable.
Works with handle_expansion().

Example: After a command exits with status 1:
$? -> "1"
$0 -> "bleshell"
*/
char	*handle_special_var(const char *var_name, t_vars *vars)
{
    if (!var_name || !*var_name)
        return (ft_strdup(""));
    if (!ft_strcmp(var_name, "?"))
        return (chk_exitstatus(vars));
    if (!ft_strcmp(var_name, "0"))
        return (ft_strdup("bleshell"));
    return (NULL);
}

/*
Retrieves value of an environment variable from vars->env.
- Searches through environment array for matching variable.
- Extracts value portion after the '=' character.
- Returns empty string for missing variables.
Returns:
Newly allocated string containing variable value.
Empty string if variable not found or on errors.
Works with handle_expansion().

Example: For "HOME=/Users/bleow" in environment:
get_env_val("HOME", env) -> "/Users/bleow"
get_env_val("NONEXISTENT", env) -> ""
*/
char	*get_env_val(const char *var_name, char **env)
{
    int		i;
    size_t	var_len;

    if (!var_name || !*var_name || !env)
        return (ft_strdup(""));
    var_len = ft_strlen(var_name);
    i = 0;
    while (env[i])
    {
        if (!ft_strncmp(env[i], var_name, var_len)
            && env[i][var_len] == '=')
            return (ft_strdup(env[i] + var_len + 1));
        i++;
    }
    return (ft_strdup(""));
}

/*
Extracts variable name from input string.
- Takes input string and current position.
- Extracts variable name (alphanumeric + underscore).
- Updates position to point after the variable name.
Returns:
Newly allocated string containing variable name.
NULL on memory allocation failure.
Works with handle_expansion().
*/
char	*get_var_name(char *input, int *pos)
{
    int		start;
    char	*var_name;

    start = *pos;
    while (input[*pos] && (ft_isalnum(input[*pos]) || input[*pos] == '_'))
        (*pos)++;
    var_name = ft_substr(input, start, *pos - start);
    return (var_name);
}

/*
Processes environment variable expansion.
- Checks for $ character at current position.
- Extracts variable name following the $.
- Handles special vars or environment vars.
- Updates position to after the expanded variable.
Returns:
Newly allocated string with expanded value.
NULL if not a variable or on allocation failure.
Works with lexerlist() and expand_cmd_args().

Example: Input: "$HOME/file" at position 0
- Recognizes $ character
- Extracts "HOME" as variable name
- Returns value (e.g., "/Users/username")
- Updates position to 5 (after "HOME")
*/
char	*handle_expansion(char *input, int *pos, t_vars *vars)
{
    char	*var_name;
    char	*value;

    if (input && input[*pos] == '$')
        fprintf(stderr, "DEBUG: Processing %s token\n", 
            get_token_str(TYPE_EXPANSION));
    if (!input || input[*pos] != '$')
        return (NULL);
    (*pos)++;
    var_name = get_var_name(input, pos);
    if (!var_name)
        return (NULL);
    value = handle_special_var(var_name, vars);
    if (!value)
        value = get_env_val(var_name, vars->env);
    ft_safefree((void **)&var_name);
    return (value);
}

/*
Processes a single argument for expansion.
- Checks if argument starts with $.
- Handles expansion of the variable.
- Replaces original argument with expanded value.
Returns:
1 if argument was expanded.
0 if no expansion occurred.
Works with expand_cmd_args().
*/
int	expand_one_arg(char **arg, t_vars *vars)
{
    int		pos;
    char	*expanded;

    if (!arg || !*arg || (*arg)[0] != '$')
        return (0);
    pos = 0;
    expanded = handle_expansion(*arg, &pos, vars);
    if (!expanded)
        return (0);
    ft_safefree((void **)&(*arg));
    *arg = expanded;
    return (1);
}

/*
Expands all command arguments containing environment variables.
- Iterates through all arguments in a command node.
- Replaces arguments starting with $ with their expanded values.
- Preserves non-variable arguments.
Works with execute_cmd() and related functions.

Example: For command node with args ["ls", "$HOME", "-l"]:
- Expands "$HOME" to "/Users/username"
- Results in args ["ls", "/Users/username", "-l"]
*/
void	expand_cmd_args(t_node *node, t_vars *vars)
{
    int		i;

    if (!node || !node->args)
        return ;
    i = 0;
    while (node->args[i])
    {
        expand_one_arg(&(node->args[i]), vars);
        i++;
    }
}
