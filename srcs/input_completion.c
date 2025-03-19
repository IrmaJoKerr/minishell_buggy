/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_completion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:03:35 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 04:14:31 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Check if a command is complete.
Returns 1 if complete, 0 if more input is needed.
Complete means no unclosed quotes and no dangling pipes.
*/
int	is_input_complete(t_vars *vars)
{
	t_node	*current;
	int		expecting_command;
	
	expecting_command = 0;
	current = vars->head;
	
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			expecting_command = 1;
		}
		else if (expecting_command && current->type != TYPE_STRING 
			&& current->type != TYPE_CMD)
		{
			expecting_command = 1;
		}
		else if (expecting_command)
		{
			expecting_command = 0;
		}
		current = current->next;
	}
	return (!expecting_command);
}

/*
Check if there's an unfinished pipe at the end of input.
Returns:
- 1 if there's an unfinished pipe needing completion
- 0 if there's no unfinished pipe
Works with handle_unfinished_pipes().
*/
int	check_unfinished_pipe(t_vars *vars, t_ast *ast)
{
	t_node	*last_token;
	t_node	*current;
	
	last_token = NULL;
	current = vars->head;
	// Find the last token
	while (current)
	{
		last_token = current;
		current = current->next;
	}
	// Check if the last token is a pipe
	if (last_token && last_token->type == TYPE_PIPE)
	{
		// If we have an AST struct, update its state
		if (ast)
			ast->pipe_at_end = 1;
		return 1;
	}
	// Also check the AST pipe_at_end flag if already set
	if (ast && ast->pipe_at_end)
		return 1;
	
	return 0;
}

/*
Check for unfinished pipes in input and handle them.
Prompts for additional input as needed.
Returns:
- 1 if pipes were handled and modifications were made
- 0 if no unfinished pipes found
- -1 if an error occurred
*/
int	handle_unfinished_pipes(char **processed_cmd, t_vars *vars, t_ast *ast)
{
	char *additional;
	
	if (!check_unfinished_pipe(vars, ast))
		return (0);
	
	print_error("Pipe at end of input", NULL, 0);
	additional = readline("pipe> ");
	
	if (!additional)
	{
		/* User pressed Ctrl+D during pipe input */
		ft_safefree((void **)processed_cmd);
		return (-1);
	}
	*processed_cmd = append_new_input(*processed_cmd, additional);
	ft_safefree((void **)&additional);
	
	if (!*processed_cmd)
		return (-1);
	return (1);
}

/*
Gets additional input for unclosed quotes with appropriate prompt.
Returns:
- The input string.
- NULL on EOF/error.
*/
char	*get_quote_input(t_vars *vars)
{
	char	*addon;
	char	*prompt;
	char	quote_char;
	
	// Determine quote char and type for proper prompt and debugging
	quote_char = vars->quote_ctx[vars->quote_depth - 1].type;
	// Output debug information directly
	fprintf(stderr, "DEBUG: Unclosed %s quote detected (depth: %d)\n",
		(quote_char == '\'' ? "single" : "double"), vars->quote_depth);
	print_error("Unclosed quotes detected", NULL, 0);
	// Set up proper bash-style prompt
	if (quote_char == '\'')
		prompt = "SQUOTE> ";
	else
		prompt = "DQUOTE> ";
	// Read additional input
	addon = readline(prompt);
	if (!addon)
	{
		fprintf(stderr, "DEBUG: Received EOF during quote completion\n");
		return (NULL);
	}
	fprintf(stderr, "DEBUG: addon input for quote: '%s'\n", addon);
	return (addon);
}

/*
Process a single quote completion cycle.
Returns:
- 1 if succeeded and more processing needed
- 0 if succeeded and no more processing needed
- -1 if an error occurred
*/
int	check_quotes_closed(char **processed_cmd, t_vars *vars)
{
	char	*addon;
	int		result;
	
	addon = get_quote_input(vars);
	if (!addon)
	{
		ft_safefree((void **)processed_cmd);
		return (-1);
	}
	// Append the addon input to the current command
	*processed_cmd = append_new_input(*processed_cmd, addon);
	ft_safefree((void **)&addon);
	
	if (!*processed_cmd)
	{
		fprintf(stderr, "DEBUG: Failed to append addon input\n");
		return (-1);
	}
	// Re-tokenize to check if quotes are now closed
	if (tokenize_to_test(*processed_cmd, vars) < 0)
	{
		fprintf(stderr, "DEBUG: Tokenization failed after adding quote input\n");
		return (-1);
	}
	// Determine if more processing is needed
	if (vars->quote_depth > 0)
		result = 1;
	else
		result = 0;
	return (result);
}

/*
Check for unclosed quotes in input and handle them.
Prompts for addon input as needed until all quotes are closed.
Returns:
- 1 if quotes were handled and modifications were made
- 0 if no unclosed quotes found
- -1 if an error occurred
*/
int	handle_unclosed_quotes(char **processed_cmd, t_vars *vars)
{
	int		quote_handled;
	int		result;
	
	quote_handled = 0;
	// Keep handling quotes until they're all closed
	while (vars->quote_depth > 0)
	{
		result = check_quotes_closed(processed_cmd, vars);
		if (result < 0)
			return (-1);
		quote_handled = 1;
		// If no more processing needed, break
		if (result == 0)
			break ;
	}
	if (quote_handled == 1)
	{
		fprintf(stderr, "DEBUG: All quotes handled, input modified\n");
		return (1);
	}
	fprintf(stderr, "DEBUG: No unclosed quotes detected\n");
	return (0);
}

/*
Append new input to existing input with a newline.
Returns the combined string or NULL on error.
Frees old input if new allocation succeeds.
*/
char	*append_input(char *old_input, char *additional)
{
	char	*tmp;
	char	*with_newline;
	char	*result;
	
	if (!additional)
		return (old_input);
	/* First append a newline to the old input */
	tmp = old_input;
	with_newline = ft_strjoin(old_input, "\n");
	if (!with_newline)
		return (old_input);
	ft_safefree((void **)&tmp);
	/* Then append the additional input */
	result = ft_strjoin(with_newline, additional);
	ft_safefree((void **)&with_newline);
	ft_safefree((void **)&additional);
	if (!result)
		return (NULL);
	return (result);
}

/*
Handles unclosed quotes. Prompts the user to close the quotes with new input.
1) Checks which type of quote is unclosed (single or double)
2) Displays matching prompt (SQUOTE> or DQUOTE>)
3) Reads new input from user with readline()
4) Joins the new input with the existing input string using ft_strjoin()
5) Retokenizes the combined input to check if quotes are now closed
6) Repeats until all quotes are closed (quote_depth == 0)
Returns the completed input string with balanced quotes, or NULL on error.
Works with tokenize() and lexerlist().
OLD VERSION
char	*handle_unclosed_quotes(char *input, t_vars *vars)
{
	char	*line;
	char	*temp;
	char	*prompt;
	char	*result;

	prompt = "DQUOTE> ";
	if (vars->quote_depth > 0
		&& vars->quote_ctx[vars->quote_depth - 1].type == '\'')
		prompt = "SQUOTE> ";
	while (vars->quote_depth > 0)
	{
		line = readline(prompt);
		if (!line)
			return (NULL);
		temp = input;
		result = ft_strjoin(temp, "\n");
		ft_safefree((void **)&temp);
		temp = result;
		result = ft_strjoin(temp, line);
		ft_safefree((void **)&temp);
		ft_safefree((void **)&line);
		input = result;
		tokenize(input, vars);
	}
	return (input);
}
*/
/*
Check for unclosed quotes in input and handle them.
Prompts for addon input as needed until all quotes are closed.
Returns:
- 1 if quotes were handled and modifications were made
- 0 if no unclosed quotes found
- -1 if an error occurred
OLD VERSION
int handle_unclosed_quotes(char **processed_cmd, t_vars *vars, t_ast *ast)
{
	char	*addon;
	char	*quote_type;
	int		quote_handled;
	
	quote_handled = 0;
	// Keep handling quotes until they're all closed
	while (vars->quote_depth > 0)
	{
		// Determine quote type for better debugging
		if (vars->quote_ctx[vars->quote_depth - 1].type == '\'')
			quote_type = "single";
		else
			quote_type = "double";
		fprintf(stderr, "DEBUG: Unclosed %s quote detected (depth: %d)\n", 
				quote_type, vars->quote_depth);
		print_error("Unclosed quotes detected", NULL, 0);
		// Use the correct prompt based on quote type
		if (vars->quote_ctx[vars->quote_depth - 1].type == '\'')
			addon = readline("quote> ");
		else
			addon = readline("quote> ");
		if (!addon)
		{
			// User pressed Ctrl+D during quote input
			fprintf(stderr, "DEBUG: Received EOF during quote completion\n");
			ft_safefree((void **)processed_cmd);
			return (-1);
		}
		fprintf(stderr, "DEBUG: addon input for quote: '%s'\n", addon);
		*processed_cmd = append_new_input(*processed_cmd, addon);
		ft_safefree((void **)&addon);
		if (!*processed_cmd)
		{
			fprintf(stderr, "DEBUG: Failed to append addon input\n");
			return (-1);
		}
		// Re-tokenize to check if quotes are now closed
		if (tokenize_to_test(*processed_cmd, vars) < 0)
		{
			fprintf(stderr, "DEBUG: Tokenization failed after adding quote input\n");
			return (-1);
		}
		quote_handled = 1;
	}
	if (quote_handled == 1)
	{
		fprintf(stderr, "DEBUG: All quotes handled, input modified\n");
		return (1);
	}
	fprintf(stderr, "DEBUG: No unclosed quotes detected\n");
	return (0);
}
*/
