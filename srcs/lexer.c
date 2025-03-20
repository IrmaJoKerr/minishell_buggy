/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:17:46 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 16:12:42 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Skips whitespace characters in the input string.
- Moves past spaces, tabs, and newlines.
- Updates vars->pos to point to the next non-whitespace.
Returns:
Nothing (void function).
Works with lexerlist() to handle token separation.

Example: Input "  echo hello"
- Skips the two spaces at the beginning
- Sets vars->pos to 2 (pointing at 'e')
*/
void	skip_whitespace(char *str, t_vars *vars)
{
    while (str[vars->pos] && (str[vars->pos] == ' '
            || str[vars->pos] == '\t' || str[vars->pos] == '\n'))
        vars->pos++;
}

/*
Prompts for additional input when input is incomplete.
- Used when quotes are unclosed or pipes need more input.
- Provides visual indicator of input continuation.
Returns:
Allocated string with user input or NULL if readline fails.
Works with handle_unclosed_quotes() and handle_unfinished_pipes().

Example: read_added_input("quote> ")
- Shows "quote> " prompt to user
- Returns user input as newly allocated string
*/
char	*read_added_input(char *prompt)
{
    char	*line;
    
    line = readline(prompt);
    if (!line)
        return (NULL);
    return (line);
}

/*
Processes a text segment before reaching a special character.
- Creates a command token for the text segment.
- Updates token list with the new command node.
Returns:
Nothing (void function).
Works with handle_operator_token().

Example: For input "echo hello | grep"
- When reaching pipe, processes "echo hello" as command
- Creates command node and adds to token list
*/
void	handle_text_chunk(char *str, t_vars *vars)
{
    char	*cmd_str;

    if (vars->pos <= vars->start)
        return ;
    cmd_str = ft_substr(str, vars->start, vars->pos - vars->start);
    if (!cmd_str)
        return ;
    vars->curr_type = TYPE_CMD;
    fprintf(stderr, "DEBUG: Processing text chunk: '%s'\n", cmd_str);
    process_cmd_token(cmd_str, vars);
    ft_safefree((void **)&cmd_str);
}

/*
Processes text and adds to token list with optional type override.
- Creates token from current text segment.
- Applies standard or override token type.
- Updates token list and position markers.
Returns:
Nothing (void function).
Works with handle_token_boundary() and handle_token().

Example: With first_token=1 and TYPE_CMD type
- Creates command token for current text segment
- Sets first_token to 0 after processing
OLD VERSION
void	process_text(char *str, t_vars *vars, int *first_token, 
                    t_tokentype override_type)
{
    char	*text;
    
    if (vars->pos <= vars->start)
        return ;
    text = ft_substr(str, vars->start, vars->pos - vars->start);
    if (!text)
        return ;
    vars->curr_type = override_type ? override_type : TYPE_STRING;
    if (*first_token)
    {
        vars->curr_type = TYPE_CMD;
        *first_token = 0;
    }
    fprintf(stderr, "DEBUG: Processing text: '%s' as %s\n", 
        text, get_token_str(vars->curr_type));
    
    if (vars->curr_type == TYPE_CMD)
        process_cmd_token(text, vars);
    else
        maketoken(text, vars);
    ft_safefree((void **)&text);
    vars->start = vars->pos;
}
*/
// Add this to your tokenize.c or wherever your process_text function is located
void process_text(char *str, t_vars *vars, int *first_token, t_tokentype override_type)
{
    char *token;
    int len = vars->pos - vars->start;
    
    if (len <= 0)
        return;
    
    token = ft_substr(str, vars->start, len);
    if (!token)
        return;
    
    fprintf(stderr, "DEBUG: process_text: len=%d, text='%s', first_token=%d\n", 
            len, token, *first_token);
    
    // If this is the first token, it's a command
    if (*first_token)
    {
        fprintf(stderr, "DEBUG: Classifying as command\n");
        vars->curr_type = TYPE_CMD;
        *first_token = 0;
    }
    // If override_type is provided, use it
    else if (override_type != TYPE_NULL)
    {
        fprintf(stderr, "DEBUG: Using override type %d\n", override_type);
        vars->curr_type = override_type;
    }
    // Otherwise, if token starts with '-', it's an argument to previous command
    else if (token[0] == '-')
    {
        fprintf(stderr, "DEBUG: Classifying as argument to command '%s'\n", 
                vars->head ? vars->head->args[0] : "unknown");
        vars->curr_type = TYPE_ARGS;
    }
    // For all other cases, determine based on context
    else
    {
        fprintf(stderr, "DEBUG: Classifying as STRING\n");
        vars->curr_type = TYPE_STRING;
        
        // After a pipe, the next string is likely a command
        if (vars->current && vars->current->type == TYPE_PIPE)
        {
            fprintf(stderr, "DEBUG: Reclassifying after pipe as CMD\n");
            vars->curr_type = TYPE_CMD;
        }
    }
    
    fprintf(stderr, "DEBUG: process_text: extracted token='%s'\n", token);
    maketoken(token, vars);
    ft_safefree((void **)&token);
}

/*
Processes quoted content in the input string.
- Handles content inside single or double quotes.
- Updates quote depth and context tracking.
Returns:
Nothing (void function).
Works with handle_token().

Example: For input with "'Hello world'"
- Tracks single quote context
- Processes "Hello world" as quoted content
*/
void	handle_quote_content(char *str, t_vars *vars, int *first_token)
{
    if (vars->pos > vars->start)
        process_text(str, vars, first_token, 0);
    
    handle_quotes(str, &vars->pos, vars);
}

/*
Handles incomplete quoted input by reading additional lines.
- Provides continuation prompts with quote> prefix.
- Appends new input to existing command string.
- Re-tokenizes the combined input to check quote closure.
Returns:
- Modified string with complete quoted content
- NULL on memory allocation failure or input error
Works with lexerlist() for handling unclosed quotes.

Example: For input with unclosed quote "echo "hello
- Prompts user with "quote> "
- User enters "world""
- Function returns "echo "hello\nworld""
- Quote depth is reset after processing
*/
char *lexing_unclosed_quo(char *input, t_vars *vars)
{
    char *processed_cmd = input;
    
    while (vars->quote_depth > 0)
    {
        char *addon = get_quote_input(vars);
        if (!addon)
            return (NULL);
        processed_cmd = append_input(processed_cmd, addon);
        ft_safefree((void **)&addon);
        
        if (!processed_cmd)
            return (NULL);
        tokenize(processed_cmd, vars);
    }
    return processed_cmd;
}

/*
Processes variable expansion tokens.
- Handles $VAR syntax for variable expansion.
- Creates appropriate expansion nodes.
Returns:
Nothing (void function).
Works with handle_token().

Example: For $HOME in input
- Creates expansion token for HOME variable
- Updates position to end of variable name
*/
void	handle_expansion_token(char *str, t_vars *vars, int *first_token)
{
    char	*expanded;
    
    if (vars->pos > vars->start)
        process_text(str, vars, first_token, 0);
    expanded = handle_expansion(str, &vars->pos, vars);
    if (expanded)
        ft_safefree((void **)&expanded);
    
    vars->start = vars->pos;
}

/*
Handles token boundary at whitespace or end of input.
- Processes any accumulated text before boundary.
- Updates position markers for next token.
Returns:
Nothing (void function).
Works with handle_token().

Example: For "echo hello" when reaching space
- Processes "echo" as command token
- Updates position markers to start after space
*/
void	handle_token_boundary(char *str, t_vars *vars, int *first_token)
{
    if (vars->pos > vars->start)
    {
        process_text(str, vars, first_token, 0);
        skip_whitespace(str, vars);
        vars->start = vars->pos;
    }
    else
    {
        skip_whitespace(str, vars);
        vars->start = vars->pos;
    }
}

/*
Creates an operator token and adds to token list.
- Handles different operator types (pipe, redirections).
- Creates appropriate node based on operator type.
Returns:
Nothing (void function).
Works with handle_token().

Example: For | operator
- Creates TYPE_PIPE node
- Adds to token linked list
*/
void	create_operator_token(t_vars *vars, t_tokentype type, char *symbol)
{
    t_node	*operator_node;
    
    operator_node = initnode(type, symbol);
    if (!operator_node)
        return ;
    vars->curr_type = type;
    build_token_linklist(vars, operator_node);
    fprintf(stderr, "DEBUG: Added %s operator token\n", 
        get_token_str(type));
}

/*
Processes operator tokens like pipe and redirections.
- Handles single and double character operators.
- Updates position and token list appropriately.
Returns:
Nothing (void function).
Works with handle_token().

Example: For input with ">" operator
- Creates output redirection token
- Updates position past the operator
*/
void	handle_operator_token(char *str, t_vars *vars, int *first_token)
{
    if (vars->pos > vars->start)
        process_text(str, vars, first_token, 0);
    if (str[vars->pos] == '|')
        create_operator_token(vars, TYPE_PIPE, "|");
    else if (str[vars->pos] == '>' && str[vars->pos + 1] == '>')
    {
        create_operator_token(vars, TYPE_APPEND_REDIRECT, ">>");
        vars->pos++;
    }
    else if (str[vars->pos] == '<' && str[vars->pos + 1] == '<')
    {
        create_operator_token(vars, TYPE_HEREDOC, "<<");
        vars->pos++;
    }
    else if (str[vars->pos] == '>')
        create_operator_token(vars, TYPE_OUT_REDIRECT, ">");
    else if (str[vars->pos] == '<')
        create_operator_token(vars, TYPE_IN_REDIRECT, "<");
    vars->pos++;
    vars->start = vars->pos;
}

/*
Processes a single token in the input string.
- Handles different token types: operators, quotes, expansions.
- Updates position and token list as tokens are processed.
Returns:
Nothing (void function).
Works with lexerlist().

Example: For input "echo $HOME"
- Processes "echo" as command token
- Processes "$HOME" as expansion token
*/
void	handle_token(char *str, t_vars *vars)
{
    int	first_token;
    
    first_token = 1;
    while (str && str[vars->pos])
    {
        if (str[vars->pos] == ' ' || str[vars->pos] == '\t' 
            || str[vars->pos] == '\n')
            handle_token_boundary(str, vars, &first_token);
        else if (vars->quote_depth == 0 && (str[vars->pos] == '|' 
            || str[vars->pos] == '>' || str[vars->pos] == '<'))
            handle_operator_token(str, vars, &first_token);
        else if ((str[vars->pos] == '\'' || str[vars->pos] == '"') 
            && vars->quote_depth == 0)
            handle_quote_content(str, vars, &first_token);
        else if (str[vars->pos] == '$' && vars->quote_depth == 0)
            handle_expansion_token(str, vars, &first_token);
        else
            vars->pos++;
    }
    if (vars->pos > vars->start)
        process_text(str, vars, &first_token, 0);
}




/*
Main lexical analysis function.
- Converts input string into linked list of tokens.
- Initializes lexer state and processes all tokens.
- Handles special cases like unclosed quotes.
Returns:
Nothing (void function).
Works with verify_input() and other high-level functions.

Example: Input "echo hello | grep world"
- Creates tokens: TYPE_CMD(echo, hello), TYPE_PIPE, TYPE_CMD(grep, world)
- Builds linked list in vars->head
*/
void	lexerlist(char *str, t_vars *vars)
{
    vars->pos = 0;
    vars->start = 0;
    vars->head = NULL;
    vars->current = NULL;
    
    fprintf(stderr, "DEBUG: Starting lexer list for: '%s'\n", str);
    
    if (!str || !*str)
        return ;
    
    handle_token(str, vars);
    
    if (vars->quote_depth > 0 && str)
        str = lexing_unclosed_quo(str, vars);
    
    if (vars->head)
        fprintf(stderr, "DEBUG: Tokens created, first: %s\n", 
            get_token_str(vars->head->type));
    else
        fprintf(stderr, "DEBUG: No tokens created!\n");
}

/*
Processes command token and arguments.
- Splits command string by whitespace.
- Creates command node with arguments.
- Updates token list.
Returns:
Nothing (void function).
Works with process_text().

Example: For "echo hello world"
- Creates TYPE_CMD node with "echo" as command
- Adds "hello" and "world" as arguments
DEPRECATED.MIGHT BE REMOVED IF FUNCTION IN TOKENIZE.C IS
void	process_cmd_token(char *input, t_vars *vars)
{
    char	**parts;
    t_node	*cmd_node;
    int		i;
    
    parts = ft_splitstr(input, " \t\n");
    if (!parts || !parts[0])
    {
        if (parts)
            ft_free_2d(parts, ft_arrlen(parts));
        return ;
    }
    cmd_node = initnode(TYPE_CMD, parts[0]);
    if (!cmd_node)
    {
        ft_free_2d(parts, ft_arrlen(parts));
        return ;
    }
    i = 1;
    while (parts[i])
    {
        append_arg(cmd_node, parts[i]);
        i++;
    }
    build_token_linklist(vars, cmd_node);
    ft_free_2d(parts, ft_arrlen(parts));
}
*/