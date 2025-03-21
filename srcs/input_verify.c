/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_verify.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:01:36 by bleow             #+#    #+#             */
/*   Updated: 2025/03/21 11:50:36 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Tokenize input for verification.
Cleans up any previous token list first.
Returns:
- 0 if tokenization was successful
- -1 if an error occurred
*/
int tokenize_to_test(char *input, t_vars *vars)
{
	fprintf(stderr, "DEBUG: Starting token verification for input '%s'\n", input);
	/* Clean up previous token list */
	cleanup_token_list(vars);
	/* Reset quote context */
	vars->quote_depth = 0;
	/* Tokenize the input */
	tokenize(input, vars);
	lexerlist(input, vars);
	fprintf(stderr, "DEBUG: Token verification complete\n");
	return (0);
}

/*
Process input modifications for quotes and pipes.
Handles unclosed quotes and pipes at the end.
Returns:
- 1 if input was modified and needs to be re-processed
- 0 if no modifications were needed
- -1 on error
*/
int process_input_addons(char **processed_cmd, t_vars *vars, t_ast *ast)
{
	int	addon_result;
	
	/* Handle unclosed quotes */
	addon_result = handle_unclosed_quotes(processed_cmd, vars);
	if (addon_result < 0)
		return (-1);
	if (addon_result > 0)
		return (1);
	/* Handle unfinished pipes */
	addon_result = handle_unfinished_pipes(processed_cmd, vars, ast);
	if (addon_result < 0)
		return (-1);
	if (addon_result > 0)
		return (1);
	/* No modifications needed */
	return (0);
}

/*
Check if input starts with a pipe token (syntax error).
Returns:
- 1 if pipe found at beginning of input
- 0 if no pipe at beginning or no tokens
Works with chk_syntax_errors().
*/
int	chk_pipe_before_cmd(t_vars *vars, t_ast *ast)
{
    if (!vars || !vars->head)
        return (0);
        
    if (vars->head->type == TYPE_PIPE)
    {
        // Update AST state if needed
        if (ast)
            ast->pipe_at_front = 1;
        return (1);
    }
    
    return (0);
}

/*
Check for consecutive pipes in token list (syntax error).
Code 258 is the standard code used for syntax errors.
Returns:
0 - no consecutive pipes found
1 - consecutive pipes found (sets error code)
*/
int	chk_serial_pipes(t_vars *vars, t_ast *ast)
{
	t_node	*current;
	
	if (!vars || !vars->head || !ast)
		return (0);
	ast->serial_pipes = 0;
	current = vars->head;
	fprintf(stderr, "DEBUG: [chk_serial_pipes] Checking for adjacent pipes\n");
	while (current)
	{
		if (current->type == TYPE_PIPE)
		{
			if (ast->serial_pipes > 0)
			{
				fprintf(stderr, "DEBUG: [chk_serial_pipes] Found adjacent pipes, setting error code %d\n", 1);
				fprintf(stderr, "DEBUG: Syntax error: consecutive pipes\n");
				if (vars->pipeline)
					vars->pipeline->last_cmdcode = 258;
				ast->syntax_error = 1;
				return (1);
			}
			ast->serial_pipes++;
		}
		else
			ast->serial_pipes = 0;
		current = current->next;
	}
	fprintf(stderr, "DEBUG: [chk_serial_pipes] Exiting with status %d\n", 1);
	return (0);
}

/*
Check for syntax errors in the input.
Returns:
  0 - No syntax errors
  1 - Syntax error detected
OLD VERSION
int	chk_syntax_errors(t_vars *vars)
{
	t_ast	*ast;
	int		result;
	
	ast = init_ast_struct(vars);
	if (!ast)
		return (1);
	// Check for pipe at beginning
	if (chk_pipe_before_cmd(vars, ast))
	{
		ft_putstr_fd("bleshell: syntax error near unexpected token `|'\n", 2);
		vars->pipeline->last_cmdcode = 258;
		cleanup_ast_struct(ast);
		return (1);
	}
	// Check for consecutive pipes
	if (chk_serial_pipes(vars, ast))
	{
		ft_putstr_fd("bleshell: syntax error near unexpected token `|'\n", 2);
		vars->pipeline->last_cmdcode = 258;
		cleanup_ast_struct(ast);
		return (1);
	}
	cleanup_ast_struct(ast);
	return (0);
}
NEWER OLD VERSION
int chk_syntax_errors(t_vars *vars)
{
    t_ast *ast;
    
	fprintf(stderr, "DEBUG: [chk_syntax_errors] Starting syntax check\n");
    ast = init_ast_struct();
    if (!ast)
        return (1);
    // Check for pipe at beginning
    if (chk_pipe_before_cmd(vars, ast))
    {
		fprintf(stderr, "DEBUG: in chk_syntax_errors - run chk_pipe_bf_cmd\n");
        fprintf(stderr, "DEBUG: [chk_syntax_errors] Pipeline state: %p\n", (void*)vars->pipeline);
		ft_putstr_fd("bleshell: syntax error near unexpected token `|'\n", 2);
        if (vars->pipeline)
            vars->pipeline->last_cmdcode = 258;
        cleanup_ast_struct(ast);
        return (1);
    }
    // Check for consecutive pipes
    if (chk_serial_pipes(vars, ast))
    {
		fprintf(stderr, "DEBUG: in chk_syntax_errors - run chk_serial_pipes\n");
        fprintf(stderr, "DEBUG: [chk_syntax_errors] Found error: Adjacent pipes\n");
		ft_putstr_fd("bleshell: syntax error near unexpected token `|'\n", 2);
        if (vars->pipeline)
            vars->pipeline->last_cmdcode = 258;
        cleanup_ast_struct(ast);
        return (1);
    }
    cleanup_ast_struct(ast);
	fprintf(stderr, "DEBUG: [chk_syntax_errors] Exiting with status %d\n", 1);
    return (0);
}
*/
int chk_syntax_errors(t_vars *vars)
{
    t_ast *ast;
    
    fprintf(stderr, "DEBUG: [chk_syntax_errors] Starting syntax check\n");
    ast = init_ast_struct();
    if (!ast)
        return (1);
        
    // Check for pipe at beginning
    if (chk_pipe_before_cmd(vars, ast))
    {
        fprintf(stderr, "DEBUG: in chk_syntax_errors - run chk_pipe_bf_cmd\n");
        fprintf(stderr, "DEBUG: [chk_syntax_errors] Pipeline state: %p\n", (void*)vars->pipeline);
        ft_putstr_fd("bleshell: syntax error near unexpected token `|'\n", 2);
        if (vars->pipeline)
            vars->pipeline->last_cmdcode = 258;
        cleanup_ast_struct(ast);
        
        // Clean up token list to prevent double free
        cleanup_token_list(vars);
        return (1);
    }
    
    // Check for consecutive pipes
    if (chk_serial_pipes(vars, ast))
    {
        fprintf(stderr, "DEBUG: in chk_syntax_errors - run chk_serial_pipes\n");
        fprintf(stderr, "DEBUG: [chk_syntax_errors] Found error: Adjacent pipes\n");
        ft_putstr_fd("bleshell: syntax error near unexpected token `|'\n", 2);
        if (vars->pipeline)
            vars->pipeline->last_cmdcode = 258;
        cleanup_ast_struct(ast);
        
        // Clean up token list to prevent double free
        cleanup_token_list(vars);
        return (1);
    }
    
    cleanup_ast_struct(ast);
    fprintf(stderr, "DEBUG: [chk_syntax_errors] Exiting with status %d\n", 0);
    return (0);
}

/*
Counts the number of tokens in a token list.
Used for debugging memory management.DEBUGGING REMOVE LATER
*/
int count_tokens(t_node *head)
{
    int count;
    t_node *current;
    
    count = 0;
    current = head;
    while (current)
    {
        count++;
        current = current->next;
    }
    return count;
}

/*
Prepare input for execution by validating and building tokens.
Handles token list cleanup, input verification, and syntax checking.
Returns:
- 0 if preparation was successful
- 1 if an error occurred
OLD VERSION
int	prepare_input(char *input, t_vars *vars, char **processed_cmd)
{
	// Clean up previous token list and AST
	cleanup_token_list(vars);
	// Reset pipeline cmd_count
	if (vars->pipeline)
		vars->pipeline->cmd_count = 0;
	// Verify the input is complete
	*processed_cmd = verify_input(input, vars);
	if (!*processed_cmd)
		return (1);
	print_error("Processing input", NULL, 0);
	// Tokenize the verified input
	tokenize(*processed_cmd, vars);
	lexerlist(*processed_cmd, vars);
	// Check for syntax errors
	if (chk_syntax_errors(vars))
	{
		ft_safefree((void **)processed_cmd);
		return (1);
	}
	print_error("Tokens processed successfully", NULL, 0);
	return (0);
}
*/
/*NEWER OLD VERSION
int prepare_input(char *input, t_vars *vars, char **processed_cmd)
{
    // Clean up previous token list and AST
    fprintf(stderr, "DEBUG: [prepare_input] Starting cleanup before processing\n");
    cleanup_token_list(vars);
    fprintf(stderr, "DEBUG: [prepare_input] Initial cleanup complete\n");
    
    // Reset pipeline cmd_count
    if (vars->pipeline)
        vars->pipeline->cmd_count = 0;
        
    // Verify the input is complete
    *processed_cmd = verify_input(input, vars);
    if (!*processed_cmd)
    {
        fprintf(stderr, "DEBUG: [prepare_input] Failed to verify input\n");
        return (1);
    }
    
    print_error("Processing input", NULL, 0);
    
    // Tokenize the verified input
    tokenize(*processed_cmd, vars);
    lexerlist(*processed_cmd, vars);
    
    // Check for syntax errors
    fprintf(stderr, "DEBUG: [prepare_input] Before syntax check, token count: %d\n",
            count_tokens(vars->head));
            
    if (chk_syntax_errors(vars))
    {
        fprintf(stderr, "DEBUG: [prepare_input] Syntax error detected, cleaning up processed_cmd %p\n",
                (void*)*processed_cmd);
                
        // Check token list state before freeing processed_cmd
        fprintf(stderr, "DEBUG: [prepare_input] Token list state after syntax error: head=%p, count=%d\n",
                (void*)vars->head, count_tokens(vars->head));
                
        ft_safefree((void **)processed_cmd);
        
        // Verify token list isn't freed again
        fprintf(stderr, "DEBUG: [prepare_input] After processed_cmd cleanup, token list: head=%p\n",
                (void*)vars->head);
                
        return (1);
    }
    
    fprintf(stderr, "DEBUG: [prepare_input] Syntax check passed, token count: %d\n",
            count_tokens(vars->head));
    
    print_error("Tokens processed successfully", NULL, 0);
    return (0);
}
*/
int prepare_input(char *input, t_vars *vars, char **processed_cmd)
{
    /* Clean up previous token list and AST */
    fprintf(stderr, "DEBUG: [prepare_input] Starting cleanup before processing\n");
    cleanup_token_list(vars);
    fprintf(stderr, "DEBUG: [prepare_input] Initial cleanup complete\n");
    
    /* Reset pipeline cmd_count */
    if (vars->pipeline)
        vars->pipeline->cmd_count = 0;
        
    /* Verify the input is complete */
    *processed_cmd = verify_input(input, vars);
    if (!*processed_cmd)
    {
        fprintf(stderr, "DEBUG: [prepare_input] Failed to verify input\n");
        return (1);
    }
    
    print_error("Processing input", NULL, 0);
    
    /* Tokenize the verified input */
    tokenize(*processed_cmd, vars);
    lexerlist(*processed_cmd, vars);
    
    /* Check for syntax errors */
    fprintf(stderr, "DEBUG: [prepare_input] Before syntax check, token count: %d\n",
            count_tokens(vars->head));
            
    if (chk_syntax_errors(vars))
    {
        fprintf(stderr, "DEBUG: [prepare_input] Syntax error detected, cleaning up processed_cmd %p\n",
                (void*)*processed_cmd);
                
        // Free the processed command string
        ft_safefree((void **)processed_cmd);
        
        // CRITICAL FIX: Set head to NULL after cleanup to prevent double free
        // This ensures the token list won't be freed again later
        vars->head = NULL;
        vars->current = NULL;
        
        fprintf(stderr, "DEBUG: [prepare_input] After syntax error cleanup, vars->head = %p\n", 
                (void*)vars->head);
        
        return (1);
    }
    
    fprintf(stderr, "DEBUG: [prepare_input] Syntax check passed, token count: %d\n",
            count_tokens(vars->head));
    
    print_error("Tokens processed successfully", NULL, 0);
    return (0);
}

int chk_input_valid(t_vars *vars, char **input)
{
    int modified;
    
	modified = 0;
    if (vars->quote_depth > 0) {
        char *result = fix_open_quotes(*input, vars);
        if (!result)
            return (0);
        if (result != *input) {
            *input = result;
            modified = 1;
        }
    }
    // Then handle pipes
    t_ast *ast = init_ast_struct();
    if (ast && !is_input_complete(vars)) {
        // Use handle_pipe_valid from minishell.c
        char *result = handle_pipe_valid(*input, vars, 0);
        if (result && result != *input) {
            ft_safefree((void **)input);
            *input = result;
            modified = 1;
        }
    }
    cleanup_ast_struct(ast);
    return modified;
}

/*
Process the input to ensure it represents a complete command.
Prompts for additional input when needed.
Returns the complete input string or NULL on error.
*/
char	*verify_input(char *input, t_vars *vars)
{
	char	*complete_input;
	int		modified;
	
	complete_input = ft_strdup(input);
	if (!complete_input)
		return (NULL);
	modified = 0;
	/* Clear previous token list */
	cleanup_token_list(vars);
	/* First tokenization */
	tokenize(complete_input, vars);
	lexerlist(complete_input, vars);
	/* Check for unclosed quotes or incomplete pipes */
	while (vars->quote_depth > 0 || !is_input_complete(vars))
	{
		modified = chk_input_valid(vars, &complete_input);
		if (!complete_input)
			return (NULL);
		/* Re-tokenize with the new input */
		cleanup_token_list(vars);
		tokenize(complete_input, vars);
		lexerlist(complete_input, vars);
	}
	if (!modified)  /* No changes were made */
	{
		ft_safefree((void **)&complete_input);
		return (input);
	}
	return (complete_input);
}

/*
Joins two strings with a newline character between them.
Takes ownership of first string and frees it.
Returns the joined string or NULL on failure.
*/
char	*join_with_newline(char *first, char *second)
{
	char	*with_newline;
	char	*result;
	
	with_newline = ft_strjoin(first, "\n");
	ft_safefree((void **)&first);
	if (!with_newline)
	{
		fprintf(stderr, "DEBUG: Failed to join with newline\n");
		return (NULL);
	}
	result = ft_strjoin(with_newline, second);
	ft_safefree((void **)&with_newline);
	
	if (!result)
		fprintf(stderr, "DEBUG: Failed to join with second string\n");
	else
		fprintf(stderr, "DEBUG: Successfully appended new input\n");
	return (result);
}

/*
Helper function to append input with newline.
Takes ownership of first string and frees it after joining.
Returns a newly allocated string with combined content.
OLD VERSION
char	*append_new_input(char *first, char *second)
{
	char	*result;
	
	fprintf(stderr, "DEBUG: Appending new input\n");
	// Handle NULL first string
	if (!first)
	{
		fprintf(stderr, "DEBUG: First string is NULL, duplicating second\n");
		if (!second)
			return (NULL);
		result = ft_strdup(second);
		if (!result)
			fprintf(stderr, "DEBUG: Failed to duplicate second string\n");
		return (result);
	}
	// Handle NULL second string
	if (!second)
	{
		fprintf(stderr, "DEBUG: Second string is NULL, returning first\n");
		return (first);
	}
	// Join strings with newline
	return (join_with_newline(first, second));
}
*/
char *append_new_input(char *first, char *second)
{
    char *result;
    
    fprintf(stderr, "DEBUG: [append_new_input] first=%p, second=%p\n", (void*)first, (void*)second);
    
    if (!first)
        return (ft_strdup(second));
    if (!second)
        return (first);  // IMPORTANT: Return first without freeing if second is NULL
    
    // Create the new combined string
    result = join_with_newline(first, second);
    fprintf(stderr, "DEBUG: [append_new_input] Result=%p\n", (void*)result);
    return result;
}

/*
Tokenize input for verification.
Cleans up any previous token list first.
Returns:
- 0 if tokenization was successful
- -1 if an error occurred
OLD VERSION
int tokenize_to_test(char *input, t_vars *vars)
{
	// Clean up previous token list
	cleanup_token_list(vars);
	// Reset quote context
	vars->quote_depth = 0;
	// Tokenize the input
	tokenize(input, vars);
	lexerlist(input, vars);
	return (0);
}
*/

/*
Helper function to append input with newline.
Takes ownership of first string and frees it after joining.
Returns a newly allocated string with combined content.
OLD VERSION
char	*append_new_input(char *first, char *second)
{
	char	*with_newline;
	char	*result;
	
	fprintf(stderr, "DEBUG: Appending new input\n");
	if (!first)
	{
		fprintf(stderr, "DEBUG: First string is NULL, duplicating second\n");
		return (ft_strdup(second));
	}
	if (!second)
	{
		fprintf(stderr, "DEBUG: Second string is NULL, returning first\n");
		return (first);
	}
	with_newline = ft_strjoin(first, "\n");
	ft_safefree((void **)&first);
	if (!with_newline)
	{
		fprintf(stderr, "DEBUG: Failed to join with newline\n");
		return (NULL);
	}
	result = ft_strjoin(with_newline, second);
	ft_safefree((void **)&with_newline);
	if (!result)
		fprintf(stderr, "DEBUG: Failed to join with second string\n");
	else
		fprintf(stderr, "DEBUG: Successfully appended new input\n");
	return (result);
}
*/
