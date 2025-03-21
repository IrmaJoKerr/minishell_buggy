/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_completion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:03:35 by bleow             #+#    #+#             */
/*   Updated: 2025/03/21 03:57:52 by bleow            ###   ########.fr       */
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
OLD VERSION
int	handle_unfinished_pipes(char **processed_cmd, t_vars *vars, t_ast *ast)
{
	char *additional;
	
	if (!check_unfinished_pipe(vars, ast))
		return (0);
	
	print_error("Pipe at end of input", NULL, 0);
	additional = readline("pipe> ");
	
	if (!additional)
	{
		// User pressed Ctrl+D during pipe input
		ft_safefree((void **)processed_cmd);
		return (-1);
	}
	*processed_cmd = append_new_input(*processed_cmd, additional);
	ft_safefree((void **)&additional);
	
	if (!*processed_cmd)
		return (-1);
	return (1);
}
*/
int handle_unfinished_pipes(char **processed_cmd, t_vars *vars, t_ast *ast)
{
    char *addon_input = NULL;
    char *tmp = NULL;
    
    fprintf(stderr, "DEBUG: Checking for unfinished pipe\n");
    if (!check_unfinished_pipe(vars, ast))
        return (0);
    fprintf(stderr, "DEBUG: Found pipe at end, prompting for more input\n");
    ft_putstr_fd("bleshell: Pipe at end of input\n", 2);
    addon_input = readline("PIPE> ");
    if (!addon_input)
	{
        fprintf(stderr, "DEBUG: EOF at pipe prompt, aborting\n");
        return -1;
    }
    tmp = ft_strtrim(addon_input, " \t\n");
    free(addon_input); // Free original before reassignment
    addon_input = tmp;
    if (!addon_input || addon_input[0] == '\0') {
        fprintf(stderr, "DEBUG: Empty input, exiting\n");
        free(addon_input); // Free if empty
        return handle_unfinished_pipes(processed_cmd, vars, ast); // Try again
    }
    fprintf(stderr, "DEBUG: Appending new input: '%s'\n", addon_input);
    tmp = ft_strjoin(*processed_cmd, " ");
    if (!tmp) {
        free(addon_input);
        return -1;
    }
    char *combined = ft_strjoin(tmp, addon_input);
    free(tmp);         // Free intermediate string
    free(addon_input); // Free additional input
    if (!combined)
        return (-1);
    free(*processed_cmd);
    *processed_cmd = combined;
    fprintf(stderr, "DEBUG: Successfully appended new input\n");
    fprintf(stderr, "DEBUG: New combined command: '%s'\n", *processed_cmd);
    cleanup_token_list(vars);
    tokenize(*processed_cmd, vars);
    lexerlist(*processed_cmd, vars);
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
	fprintf(stderr, "DEBUG: addon input for quote by get_quo_iput: '%s'\n", addon);
	return (addon);
}

/*
Process a single quote completion cycle.
Returns:
- 1 if succeeded and more processing needed
- 0 if succeeded and no more processing needed
- -1 if an error occurred
*/
int	chk_quotes_closed(char **processed_cmd, t_vars *vars)
{
	char	*addon;
	int		result;
	
	addon = get_quote_input(vars);
	fprintf(stderr, "DEBUG: get_quo_put fr chk_quo_clsd\n");
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
Checks if quotes in a string are properly balanced.
- Handles both double and single quotes
- Properly tracks quote context
- Returns 1 if balanced, 0 if unbalanced
*/
int	quotes_are_closed(const char *str)
{
    int in_double_quote = 0;
    int in_single_quote = 0;
    int i = 0;
    
    in_double_quote = 0;
    in_single_quote = 0;
    i = 0;
    if (!str)
        return (1);
    while (str[i])
    {
        if (in_single_quote && str[i] == '\'')
            in_single_quote = 0;
        else if (in_double_quote && str[i] == '"')
            in_double_quote = 0;
        else if (!in_single_quote && !in_double_quote)
        {
            if (str[i] == '\'')
                in_single_quote = 1;
            else if (str[i] == '"')
                in_double_quote = 1;
        }
        i++;
    }
    return (!in_single_quote && !in_double_quote);
}

/*
Check for unclosed quotes in input and handle them.
Prompts for addon input as needed until all quotes are closed.
Returns:
- 1 if quotes were handled and modifications were made
- 0 if no unclosed quotes found
- -1 if an error occurred
OLD VERSION
int	handle_unclosed_quotes(char **processed_cmd, t_vars *vars)
{
	int		quote_handled;
	int		result;
	
	quote_handled = 0;
	// Keep handling quotes until they're all closed
	while (vars->quote_depth > 0)
	{
		result = chk_quotes_closed(processed_cmd, vars);
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
*/
/*NEWER OLD VERSION
int handle_unclosed_quotes(char **processed_cmd, t_vars *vars)
{
	int	quote_handled;
    int	result;
	
    if (quotes_are_closed(*processed_cmd))
    {
        fprintf(stderr, "DEBUG: Quotes are balanced, no completion needed\n");
        vars->quote_depth = 0;
        return (0);
    }
    quote_handled = 0;
    while (vars->quote_depth > 0)
    {
        result = chk_quotes_closed(processed_cmd, vars);
        if (result < 0)
            return (-1);
        quote_handled = 1;
        if (result == 0)
            break;
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
/*HOPEFULLY LAST OLD VERSION
int handle_unclosed_quotes(char **processed_cmd, t_vars *vars)
{
    // Remove unused variable 'result' from here
    char	*addon;
    int		quote_handled;
    
    // First check if quotes are actually balanced using quotes_are_closed
    if (quotes_are_closed(*processed_cmd))
    {
        fprintf(stderr, "DEBUG: Quotes are balanced in command, no addon needed\n");
        // Reset quote depth since quotes are actually balanced
        vars->quote_depth = 0;
        return (0);
    }
    
    fprintf(stderr, "DEBUG: Unbalanced quotes detected, prompting for more input\n");
    
    // Continue with quote handling - use regular while loop (not do-while)
    quote_handled = 0;
    while (vars->quote_depth > 0)
    {
        addon = get_quote_input(vars);
        if (!addon)
        {
            ft_safefree((void **)processed_cmd);
            return (-1);
        }
        
        fprintf(stderr, "DEBUG: addon input for quote: '%s'\n", addon);
        
        // Create a new buffer to hold the combined strings
        char *new_cmd = append_new_input(*processed_cmd, addon);
        
        // IMPORTANT: Properly handle the memory to avoid double free
        ft_safefree((void **)&addon);
        
        if (!new_cmd)
        {
            fprintf(stderr, "DEBUG: Failed to append addon input\n");
            return (-1);
        }
        
        // Replace the old command with the new one
        ft_safefree((void **)processed_cmd);
        *processed_cmd = new_cmd;
        
        // Re-tokenize to check if quotes are now closed
        if (tokenize_to_test(*processed_cmd, vars) < 0)
        {
            fprintf(stderr, "DEBUG: Tokenization failed after adding quote input\n");
            return (-1);
        }
        
        quote_handled = 1;
        
        // Determine if more processing is needed
        if (vars->quote_depth == 0)
        {
            break;  // No more quote completion needed
        }
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
/*
Check for unclosed quotes in input and handle them.
Prompts for addon input as needed until all quotes are closed.
Returns:
- 1 if quotes were handled and modifications were made
- 0 if no unclosed quotes found
- -1 if an error occurred
*/
/*
Check for unclosed quotes in input and handle them.
Prompts for addon input as needed until all quotes are closed.
Returns:
- 1 if quotes were handled and modifications were made
- 0 if no unclosed quotes found
- -1 if an error occurred
*/
int handle_unclosed_quotes(char **processed_cmd, t_vars *vars)
{
    char *addon;
    char *new_cmd;
    int quote_handled;
    
    /* Check if quotes are already balanced */
    if (quotes_are_closed(*processed_cmd))
    {
        fprintf(stderr, "DEBUG: Quotes are balanced in command, no addon needed\n");
        vars->quote_depth = 0;
        return (0);
    }
    
    fprintf(stderr, "DEBUG: Unbalanced quotes detected, prompting for more input\n");
    
    /* Initialize quote handling flag */
    quote_handled = 0;
    
    /* Continue handling quotes until all are closed */
    while (vars->quote_depth > 0)
    {
        /* Get additional input for the quote */
        addon = get_quote_input(vars);
        fprintf(stderr, "DEBUG: get_quo_put fr hdle_uncl_quo\n");
        
        /* Handle EOF or error */
        if (!addon)
        {
            fprintf(stderr, "DEBUG: Failed to get quote input\n");
            ft_safefree((void **)processed_cmd);
            return (-1);
        }
        
        fprintf(stderr, "DEBUG: addon input for quote: '%s'\n", addon);
        
        /* Create combined command */
        new_cmd = append_input(*processed_cmd, addon);
        
        /* Free addon to prevent double free */
        ft_safefree((void **)&addon);
        
        /* Handle append failure */
        if (!new_cmd)
        {
            fprintf(stderr, "DEBUG: Failed to append addon input\n");
            return (-1);
        }
        
        /* Update command with new combined version */
        ft_safefree((void **)processed_cmd);
        *processed_cmd = new_cmd;
        
        /* Re-tokenize to check if quotes are now closed */
        if (tokenize_to_test(*processed_cmd, vars) < 0)
        {
            fprintf(stderr, "DEBUG: Tokenization failed after adding quote input\n");
            return (-1);
        }
        
        /* Mark that we handled quotes */
        quote_handled = 1;
        
        /* Exit if no more quotes to process */
        if (vars->quote_depth == 0)
        {
            break;
        }
    }
    
    /* Return result based on whether quotes were handled */
    if (quote_handled)
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
OLD VERSION
char	*append_input(char *old_input, char *additional)
{
	char	*tmp;
	char	*with_newline;
	char	*result;
	
	if (!additional)
		return (old_input);
	// First append a newline to the old input
	tmp = old_input;
	with_newline = ft_strjoin(old_input, "\n");
	if (!with_newline)
		return (old_input);
	ft_safefree((void **)&tmp);
	// Then append the additional input
	result = ft_strjoin(with_newline, additional);
	ft_safefree((void **)&with_newline);
	ft_safefree((void **)&additional);
	if (!result)
		return (NULL);
	return (result);
}
*/
/*
Simple string append function that doesn't free inputs.
Just joins two strings with a newline in between.
*/
/*
Simple string append function that joins two strings with a newline in between.
*/
char *append_input(const char *first, const char *second)
{
    size_t	len1;
    size_t	len2;
    char	*result;
    
    /* Handle NULL inputs */
    if (!first)
        return (ft_strdup(second));
    if (!second)
        return (ft_strdup(first));
        
    /* Calculate string lengths */
    len1 = ft_strlen(first);
    len2 = ft_strlen(second);
    
    /* Allocate space for both strings plus newline and null terminator */
    result = malloc(len1 + len2 + 2);
    if (!result)
        return (NULL);
    ft_memcpy(result, first, len1);
    result[len1] = '\n';
    ft_memcpy(result + len1 + 1, second, len2);
    result[len1 + len2 + 1] = '\0';
    fprintf(stderr, "DEBUG: append_input created: '%s'\n", result);
    return (result);
}
