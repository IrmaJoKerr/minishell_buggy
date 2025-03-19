/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:13:52 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 06:27:41 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Processes operators and surrounding text in command input.
- Handles text before operators by creating string/command tokens.
- Detects and processes both single and double operators.
- Updates token start/end positions for proper tokenization.
Returns:
-New position index after operator processing.
Works with handle_token() during input tokenization.

Example: For input "echo hello > file"
- Processes "echo hello" as string/command when '>' is encountered
- Then processes '>' as an output redirection operator
- Returns position after '>' for continued processing
*/
int operators(char *input, int i, int token_start, t_vars *vars)
{
	if (i > token_start)
	{
		vars->start = token_start;
		vars->pos = i;
		vars->curr_type = TYPE_STRING;
		maketoken(input, vars);
		vars->start = i;
	}
	int next_pos = handle_double_operator(input, i, vars);
	if (next_pos != i)
		return (next_pos);
	return (handle_single_operator(input, i, vars));
}

/*
Creates string token for text preceding an operator.
- Checks if current position contains operator character.
- Creates a string token from token_start to current position.
- Updates token tracking in vars structure.
Returns:
- Current position (unchanged).
Works with operators() for text segment tokenization.

Example: For "echo|grep" at position of '|'
- Creates string token "echo" before processing pipe
- Sets up vars for next token processing
*/
int	handle_string(char *input, int i, int token_start, t_vars *vars)
{
	if ((input[i] == '>' || input[i] == '<' || input[i] == '|')
		&& i > token_start)
	{
		vars->start = token_start;
		vars->pos = i;
		vars->curr_type = TYPE_STRING;
		maketoken(input, vars);
	}
	return (i);
}

/*
Maps operator characters to their token types.
- '|' == TYPE_PIPE
- '>' == TYPE_OUT_REDIRECT
- '<' == TYPE_IN_REDIRECT
Returns:
- Token type corresponding to operator character.
- TYPE_STRING as fallback for unrecognized characters.
Works with handle_single_operator() for type determination.

Example: For '|' character
- Returns TYPE_PIPE enum value
- Allows consistent token type assignment
Used with handle_single_operator().
*/
t_tokentype	get_operator_type(char op)
{
	if (op == '|')
		return (TYPE_PIPE);
	else if (op == '>')
		return (TYPE_OUT_REDIRECT);
	else if (op == '<')
		return (TYPE_IN_REDIRECT);
	return (TYPE_STRING);
}

/*
Processes single-character operators (|, >, <).
- Creates text token for content before operator if needed.
- Sets appropriate token type using get_operator_type().
- Creates operator token and updates position tracking.
Returns:
- Position after operator (i+1).
- Unchanged position if no operator.
Works with operators() during tokenization.

Example: For '>' character in input
- Creates TYPE_OUT_REDIRECT token
- Updates position to character after '>'
- Returns updated position
*/
/*OLDER VERSION

int	handle_single_operator(char *input, int i, t_vars *vars)
{
    if (i > vars->start)
        handle_string(input, i, vars->start, vars);
    
    vars->start = i + 1;
    vars->curr_type = get_operator_type(input[i]);
    
    fprintf(stderr, "DEBUG: Operator '%c' identified as type %d (TYPE_NULL=%d, TYPE_PIPE=%d)\n", 
            input[i], vars->curr_type, TYPE_NULL, TYPE_PIPE);
    
    char op_str[2];
    op_str[0] = input[i];
    op_str[1] = '\0';
    
    fprintf(stderr, "DEBUG: Creating token with type=%d, value='%s'\n", 
            vars->curr_type, op_str);
    
    maketoken(op_str, vars);
    
    return (i + 1);
}
*/
int handle_single_operator(char *input, int i, t_vars *vars)
{
    if (i > vars->start)
        handle_string(input, i, vars->start, vars);
    
    vars->start = i + 1;
    vars->curr_type = get_operator_type(input[i]);
    
    fprintf(stderr, "DEBUG: Operator '%c' identified as type=%d (TYPE_PIPE=%d)\n", 
            input[i], vars->curr_type, TYPE_PIPE);
            
    char op_str[2];
    op_str[0] = input[i];
    op_str[1] = '\0';
    maketoken(op_str, vars);
    
    fprintf(stderr, "DEBUG: Created operator token: '%c' with type=%d\n", 
            input[i], vars->curr_type);
            
    return (i + 1);
}

/*
Processes two-character operators (>>, <<).
- Detects append redirect (>>) and heredoc (<<) operators.
- Creates appropriate token with TYPE_APPEND_REDIRECT or TYPE_HEREDOC.
- Updates token position tracking for continued processing.
Returns:
- Position after operator (i+2).
- Unchanged position if no match.
Works with operators() during tokenization.

Example: For ">>" in input
- Creates TYPE_APPEND_REDIRECT token
- Updates position to skip both characters
- Returns position after ">>"
*/
int	handle_double_operator(char *input, int i, t_vars *vars)
{
	if (input[i] == '>' && input[i + 1] == '>')
	{
		vars->start = i;
		vars->pos = i + 2;
		vars->curr_type = TYPE_APPEND_REDIRECT;
		maketoken(input, vars);
		return (i + 2);
	}
	if (input[i] == '<' && input[i + 1] == '<')
	{
		vars->start = i;
		vars->pos = i + 2;
		vars->curr_type = TYPE_HEREDOC;
		maketoken(input, vars);
		return (i + 2);
	}
	return (i);
}
