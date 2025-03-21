/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:04:06 by bleow             #+#    #+#             */
/*   Updated: 2025/03/21 04:10:00 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Tracks opening and closing of single and double quotes.
- Identifies quote characters in input.
- Manages quote context stack in vars structure.
- Updates quote depth counter for nesting.
- Handles both single and double quotes.
Works with tokenize() during input processing.

Example: Input "echo 'hello "world"'"
- Tracks single quote at beginning
- Tracks nested double quotes
- Ensures proper quote pairing
OLD VERSION
void handle_quotes(char *str, int *pos, t_vars *vars)
{
    char	quote_char;
    int		start;
    
    //Remember the quote character we're looking for
    quote_char = str[*pos];
    start = *pos;
    
    // Move past opening quote
    (*pos)++;
    
    // Find closing quote
    while (str[*pos] && str[*pos] != quote_char)
        (*pos)++;
    
    // If we found a closing quote
    if (str[*pos] == quote_char)
    {
        // Move past closing quote
        (*pos)++;
        
        // We have a complete quoted token
        vars->start = start;
        
        // DEBUG: Add this line
        fprintf(stderr, "DEBUG: Found matched quote: '%.*s'\n", 
                *pos - start, str + start);
                
        // Set proper quote type
        if (quote_char == '"')
            vars->curr_type = TYPE_DOUBLE_QUOTE;
        else
            vars->curr_type = TYPE_SINGLE_QUOTE;
            
        // CRITICAL FIX: Reset quote depth when quotes are balanced
        vars->quote_depth = 0;
        
        // Process the quoted token
        maketoken(ft_substr(str, start, *pos - start), vars);
        vars->start = *pos;
    }
    else
    {
        // Quote is unclosed
        vars->quote_depth++;
        vars->quote_ctx[vars->quote_depth - 1].type = quote_char;
        fprintf(stderr, "DEBUG: Unclosed %s quote detected (depth: %d)\n",
                (quote_char == '"' ? "double" : "single"), vars->quote_depth);
    }
}
*/
/*
Handles quote tokens in the input string.
*/
void handle_quotes(char *str, int *pos, t_vars *vars)
{
    char quote_char;
    int start;
    
    /* Remember the quote character we're looking for */
    quote_char = str[*pos];
    start = *pos;
    
    /* Move past opening quote */
    (*pos)++;
    
    /* Find closing quote */
    while (str[*pos] && str[*pos] != quote_char)
        (*pos)++;
    
    /* If we found a closing quote */
    if (str[*pos] == quote_char)
    {
        /* Move past closing quote */
        (*pos)++;
        
        /* We have a complete quoted token */
        vars->start = start;
        
        fprintf(stderr, "DEBUG: Found matched quote: '%.*s'\n", 
                *pos - start, str + start);
                
        /* CRITICAL FIX: Check if we have a command node to attach this to */
        if (vars->current && vars->current->type == TYPE_CMD)
        {
            /* Process as a string argument to the command */
            fprintf(stderr, "DEBUG: Adding quoted string as argument to command\n");
            vars->curr_type = TYPE_STRING;
        }
        else
        {
            /* Process as a standalone quoted token */
            if (quote_char == '"')
                vars->curr_type = TYPE_DOUBLE_QUOTE;
            else
                vars->curr_type = TYPE_SINGLE_QUOTE;
        }
            
        /* Reset quote depth when quotes are balanced */
        vars->quote_depth = 0;
        
        /* Process the quoted token */
        maketoken(ft_substr(str, start, *pos - start), vars);
        vars->start = *pos;
    }
    else
    {
        /* Quote is unclosed */
        vars->quote_depth++;
        vars->quote_ctx[vars->quote_depth - 1].type = quote_char;
        fprintf(stderr, "DEBUG: Unclosed %s quote detected (depth: %d)\n",
                (quote_char == '"' ? "double" : "single"), vars->quote_depth);
    }
}

/*
Prompts user for additional input to close unclosed quotes.
- Determines prompt based on unclosed quote type.
- Reads additional input lines.
- Joins new input with existing command.
- Re-tokenizes to check if quotes are now closed.
Returns:
- Completed input string with balanced quotes.
- NULL on error.
Works with process_command() for handling incomplete commands.

Example: When user enters command with unclosed quote
- For unclosed single quote: Shows "SQUOTE> " prompt
- For unclosed double quote: Shows "DQUOTE> " prompt
- Returns complete command once all quotes are closed
*/
char	*fix_open_quotes(char *input, t_vars *vars)
{
    char	*line;
    char	*result;
    char	*prompt;

    prompt = "DQUOTE> ";
    if (vars->quote_depth > 0
        && vars->quote_ctx[vars->quote_depth - 1].type == '\'')
        prompt = "SQUOTE> ";
    while (vars->quote_depth > 0)
    {
        line = readline(prompt);
        if (!line)
            return (NULL);
        result = append_input(input, line);
        if (!result)
            return (NULL);
        ft_safefree((void **)&line);
        input = result;
        tokenize(input, vars);
    }
    return (input);
}

/*
Extracts content between quote characters.
- Starts after opening quote character.
- Reads until matching closing quote or end of string.
- Creates substring without quote characters.
- Updates position past the closing quote.
Returns:
- Quoted content as new string.
- NULL if unclosed quote.
Works with handle_quote_token() during tokenization.

Example: For input "Hello 'world'" at position of first quote
- Returns "world" as extracted content
- Updates position to character after closing quote
*/
char	*read_quoted_content(char *input, int *pos, char quote)
{
    int		start;
    char	*content;

    start = *pos + 1;
    *pos = start;
    while (input[*pos] && input[*pos] != quote)
        (*pos)++;
    if (!input[*pos])
        return (NULL);
    content = ft_substr(input, start, *pos - start);
    (*pos)++;
    return (content);
}

/*
Removes outer quotes from a string.
- Checks for matching quotes at start and end.
- Creates new string without the quotes.
- Updates the original string pointer.
Works with process_args_tokens() during argument parsing.

Example: For string "\"hello\""
- Creates new string "hello"
- Frees original string
- Updates pointer to new string
*/
void	strip_quotes(char **str_ptr, char quote_char)
{
    char	*str;
    char	*new_str;
    size_t	len;

    str = *str_ptr;
    if (!str)
        return ;
    len = ft_strlen(str);
    if (len < 2)
        return ;
    if (str[0] == quote_char && str[len - 1] == quote_char)
    {
        new_str = ft_substr(str, 1, len - 2);
        if (new_str)
        {
            ft_safefree((void **)&str);
            *str_ptr = new_str;
        }
    }
}

/*
Removes outer quotes from an argument string.
- Checks for both single and double quotes.
- Creates new string without the quotes.
- Updates the original string pointer.
Works with process_args_tokens() during argument processing.

Example: For argument "'hello'"
- Creates new string "hello"
- Frees original string
- Updates pointer to new string
OLD VERSION
void	process_quotes_in_arg(char **arg)
{
    char	*str;
    size_t	len;
    
    str = *arg;
    if (!str)
        return ;
    len = ft_strlen(str);
    if (len < 2)
        return ;
    if (str[0] == '"' && str[len - 1] == '"')
    {
        strip_quotes(arg, '"');
        return ;
    }
    if (str[0] == '\'' && str[len - 1] == '\'')
    {
        strip_quotes(arg, '\'');
    }
}
*/
/*HOPEFULLY FINAL VERSION
void process_quotes_in_arg(char **arg)
{
    char *str;
    char *new_str;
    size_t len;
    
    str = *arg;
    if (!str)
        return;
    len = ft_strlen(str);
    if (len < 2)
        return;
        
    // Check for matching quotes at beginning and end
    if ((str[0] == '"' && str[len-1] == '"') ||
        (str[0] == '\'' && str[len-1] == '\''))
    {
        fprintf(stderr, "DEBUG: Removing quotes from argument: '%s'\n", str);
        new_str = ft_substr(str, 1, len-2);
        if (new_str)
        {
            free(str);
            *arg = new_str;
            fprintf(stderr, "DEBUG: Quotes removed, new arg: '%s'\n", *arg);
        }
    }
}
*/
void	process_quotes_in_arg(char **arg)
{
    char	*str;
    char	*new_str;
    size_t	len;
    
    str = *arg;
    if (!str)
        return ;
    len = ft_strlen(str);
    if (len < 2)
        return ;
        
    // Check for matching quotes at beginning and end
    if ((str[0] == '"' && str[len-1] == '"') ||
        (str[0] == '\'' && str[len-1] == '\''))
    {
        fprintf(stderr, "DEBUG: Removing quotes from argument: '%s'\n", str);
        new_str = ft_substr(str, 1, len-2);
        if (new_str)
        {
            free(str); // Use free directly since we're handling str, not *arg
            *arg = new_str;
            fprintf(stderr, "DEBUG: Quotes removed, new arg: '%s'\n", *arg);
        }
    }
}

/*
Scans for the ending quote character.
- Searches from current position for matching quote.
- Updates position to the quote if found.
- Provides validation for quoted content.
Returns:
- 1 if closing quote found.
- 0 if not found.
Works with valid_quote_token() for quote validation.

Example: For input "echo 'hello world'"
- When scanning for closing single quote
- Returns 1 when closing quote found
- Updates position to closing quote
*/
int	scan_for_endquote(char *str, int *pos, char quote_char)
{
    int	i;

    i = *pos + 1;
    while (str[i] && str[i] != quote_char)
        i++;
    if (str[i] == quote_char)
    {
        *pos = i;
        return (1);
    }
    return (0);
}

/*
Validates and processes a quoted token.
- Checks if the token has matching quotes.
- Creates appropriate token based on quote type.
- Handles content between quotes.
Works with handle_token() during tokenization.

Example: For "echo 'hello'"
- Validates 'hello' as properly quoted
- Creates token for the quoted content
- Updates position past the quotes
*/
void	valid_quote_token(char *str, t_vars *vars, int *pos, int start)
{
    char	quote_char;
    char	*token;
    int		end_pos;

    quote_char = str[start];
    end_pos = *pos;
    if (scan_for_endquote(str, &end_pos, quote_char))
    {
        token = ft_substr(str, start + 1, end_pos - start - 1);
        if (token)
        {
            vars->curr_type = (quote_char == '"') 
                ? TYPE_DOUBLE_QUOTE : TYPE_SINGLE_QUOTE;
            maketoken(token, vars);
            ft_safefree((void **)&token);
            *pos = end_pos + 1;
        }
    }
}
