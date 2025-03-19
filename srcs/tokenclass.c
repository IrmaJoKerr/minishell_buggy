/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenclass.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:28:07 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 00:01:32 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles exit status token. Returns the exit status. Works with classify.
*/
char	*handle_exit_status(t_vars *vars)
{
	if (vars && vars->pipeline)
		return (ft_itoa(vars->pipeline->last_cmdcode));
	return (ft_strdup("0"));
}

/*
Handles redirection token classification and checking.
mode: 0 for type check, 1 for classification
Returns: token type or boolean for type check.
Works with classify.
*/
t_tokentype	redirection_type(char *str, int mode, t_tokentype type, int pos)
{
	if (mode == 0 && type != TYPE_NULL)
	{
		return (type == TYPE_IN_REDIRECT || type == TYPE_OUT_REDIRECT
            || type == TYPE_APPEND_REDIRECT || type == TYPE_HEREDOC);
    }
	if (!str || !str[pos])
        return (TYPE_STRING);
	if (!str[pos+1])
	{
		if (str[pos] == '<')
			return (TYPE_IN_REDIRECT);
		return (TYPE_OUT_REDIRECT);
	}
	if (str[pos] == '<' && str[pos+1] == '<')
		return (TYPE_HEREDOC);
	if (str[pos] == '>' && str[pos+1] == '>')
		return (TYPE_APPEND_REDIRECT);
	if (str[pos] == '<')
		return (TYPE_IN_REDIRECT);
	return (TYPE_OUT_REDIRECT);
}

/*
Classify token type. Main controller function for token classification.
*/
t_tokentype classify(char *str, int pos)
{
	if (!str || !str[pos])
		return (TYPE_STRING);
	if (str[pos] == '-' && str[pos+1] && str[pos+1] != '"' && str[pos+1] != '\'')
		return (TYPE_ARGS);
	if (str[pos] == '|')
		return (TYPE_PIPE);
	if (str[pos] == '<' || str[pos] == '>')
		return (redirection_type(str, 1, TYPE_NULL, pos));
	if (str[pos] == '$')
	{
		if (str[pos+1] == '?')
			return (TYPE_EXIT_STATUS);
		return (TYPE_EXPANSION);
	}
	if (str[pos] == '"')
		return (TYPE_DOUBLE_QUOTE);
	if (str[pos] == '\'')
		return (TYPE_SINGLE_QUOTE);
	return (TYPE_STRING);
}
