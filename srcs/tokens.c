/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 04:36:48 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 05:48:41 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Makes a new node for other types of tokens.
*/
t_node	*new_other_node(char *token, t_tokentype type)
{
	t_node	*node;

	node = initnode(type, token);
	if (!node)
	{
		ft_safefree((void **)&token);
		return (NULL);
	}
	return (node);
}

/*
Process quoted tokens during lexical analysis.
Updates position pointer to after closing quote.
Creates token from the quoted content.
Example: "hello world" -> processes the token and moves position
*/
void	handle_quote_token(char *str, t_vars *vars, int *pos)
{
	char	quote_char;
	int		start;
	int		found_closing;

	if (!str || !vars || !pos)
		return ;
	quote_char = str[*pos];
	start = *pos;
	(*pos)++;
	if (quote_char == '"')
		vars->curr_type = TYPE_DOUBLE_QUOTE;
	else
		vars->curr_type = TYPE_SINGLE_QUOTE;
	fprintf(stderr, "DEBUG: Processing %s token\n", get_token_str(vars->curr_type));
	found_closing = scan_for_endquote(str, pos, quote_char);
	if (found_closing)
		valid_quote_token(str, vars, pos, start);
	else
	{
		vars->quote_depth++;
		vars->quote_ctx[vars->quote_depth - 1].type = quote_char;
	}
}

/*
Validate token type and content.
*/
int validate_token(t_node *token)
{
    if (!token)
        return (0);
        
    // Use get_token_str for debugging
    fprintf(stderr, "DEBUG: Validating token type %s\n", get_token_str(token->type));
    // Check for special token types
    if (token->type == TYPE_PIPE)
    {
        if (!token->next)
        {
            fprintf(stderr, "DEBUG: Error: pipe needs command after\n");
            return (0);
        }
    }
    // Validate redirections using redirection_type
    if (redirection_type(NULL, 0, token->type, 0))
    {
        if (!token->next)
        {
            fprintf(stderr, "DEBUG: Error: redirection needs target\n");
            return (0);
        }
    }
    return (1);
}

/*
Process quoted tokens during lexical analysis.
Updates position pointer to after closing quote.
Creates token from the quoted content.
Example: "hello world" -> processes the token and moves position
OLD VERSION
void handle_quote_token(char *str, t_vars *vars, int *pos)
{
	char		quote_char;
	int			start;
	t_tokentype	quote_type;

	if (!str || !vars || !pos)
		return ;
	quote_char = str[*pos];
	if (quote_char == '"')
		quote_type = TYPE_DOUBLE_QUOTE;
	else
		quote_type = TYPE_SINGLE_QUOTE;
	fprintf(stderr, "DEBUG: Processing %s token\n", get_token_str(quote_type));	
	start = *pos;
	(*pos)++;
	while (str[*pos])
	{
		if (str[*pos] == quote_char)
			break ;
		(*pos)++;
	}
	if (str[*pos] == quote_char)
	{
		(*pos)++;
		vars->start = start;
		vars->curr_type = quote_type;
		maketoken(str, vars);
		vars->start = *pos;
	}
	else
	{
		vars->quote_depth++;
		vars->quote_ctx[vars->quote_depth - 1].type = quote_char;
	}
}
*/
