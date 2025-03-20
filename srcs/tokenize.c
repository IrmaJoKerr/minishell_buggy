/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 06:12:16 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 16:30:00 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates a new token node and adds it to the token list.
- Creates node of current token type.
- Sets node data from the provided string.
- Adds node to linked list and updates current pointer.
Works with lexerlist() during token creation.

Example: For input "echo hello"
- When maketoken("echo") called with TYPE_CMD
- Creates command node with args[0] = "echo"
- Adds to token list as head or appends to current
*/
/*
Creates a token based on the current token type.
- For CMD tokens, processes command and arguments.
- For other tokens, creates appropriate node type.
- Updates token linked list.
Works with process_text().
OLD VERSION
void maketoken(char *token, t_vars *vars)
{
    fprintf(stderr, "DEBUG: maketoken called with token='%s', type=%d\n", 
            token, vars->curr_type);
            
    if (!token || !vars)
        return;
        
    if (vars->curr_type == TYPE_CMD)
    {
        process_cmd_token(token, vars);
        fprintf(stderr, "DEBUG: Command token processed: '%s'\n", token);
    }
    else
    {
        process_other_token(token, vars);
        fprintf(stderr, "DEBUG: Other token processed: type=%d, value='%s'\n", 
                vars->curr_type, token);
    }
    
    // Add debug info about the newly created token
    if (vars->current)
        fprintf(stderr, "DEBUG: Token created: type=%d, value='%s'\n", 
                vars->current->type, 
                (vars->current->args && vars->current->args[0]) ? 
                    vars->current->args[0] : "(null)");
}
*/
// Implement or modify maketoken in tokenize.c
void maketoken(char *token, t_vars *vars)
{
    t_node *node = NULL;
    
    fprintf(stderr, "DEBUG: maketoken called with token='%s', type=%d\n", 
            token, vars->curr_type);
    
    // For arguments, append to previous command node
    if (vars->curr_type == TYPE_ARGS && vars->head)
    {
        t_node *cmd_node = find_last_command(vars->head);
        
        if (cmd_node)
        {
            fprintf(stderr, "DEBUG: Adding argument '%s' to command '%s'\n",
                    token, cmd_node->args ? cmd_node->args[0] : "(null)");
            append_arg(cmd_node, token);
            return;
        }
    }
    
    // For strings that should be considered arguments to the previous command
    if (vars->curr_type == TYPE_STRING && vars->current && vars->current->type == TYPE_CMD)
    {
        fprintf(stderr, "DEBUG: Converting string '%s' to argument for command '%s'\n",
                token, vars->current->args ? vars->current->args[0] : "(null)");
        append_arg(vars->current, token);
        return;
    }
    
    // For command tokens
    if (vars->curr_type == TYPE_CMD)
    {
        fprintf(stderr, "DEBUG: Command token processed: '%s'\n", token);
        node = new_cmd_node(token);
    }
    // For pipe tokens
    else if (vars->curr_type == TYPE_PIPE)
    {
        fprintf(stderr, "DEBUG: Pipe token processed\n");
        node = new_other_node(token, TYPE_PIPE);
    }
    // For other tokens
    else
    {
        fprintf(stderr, "DEBUG: Other token processed: '%s', type=%d\n", 
                token, vars->curr_type);
        
        // For strings after a pipe, convert to commands
        if (vars->curr_type == TYPE_STRING && 
            vars->current && vars->current->type == TYPE_PIPE)
        {
            fprintf(stderr, "DEBUG: Converting string after pipe to command\n");
            vars->curr_type = TYPE_CMD;
            node = new_cmd_node(token);
        }
        else
        {
            // Create other token types
            node = new_other_node(token, vars->curr_type);
        }
    }
    
    // Add node to list
    if (node)
    {
        if (!vars->head)
        {
            vars->head = node;
            vars->current = node;
            fprintf(stderr, "DEBUG: Set head token: type=%d, value='%s'\n", 
                    node->type, token);
        }
        else
        {
            vars->current->next = node;
            node->prev = vars->current;
            vars->current = node;
            fprintf(stderr, "DEBUG: Added token to list: type=%d, value='%s'\n", 
                    node->type, token);
        }
        
        fprintf(stderr, "DEBUG: New token added: type=%d, value='%s'\n", 
                vars->curr_type, token);
    }
    else
    {
        fprintf(stderr, "DEBUG: Failed to create token for '%s'\n", token);
    }
}

// Helper function to find the last command node
t_node *find_last_command(t_node *head)
{
    t_node *current = head;
    t_node *last_cmd = NULL;
    
    while (current)
    {
        if (current->type == TYPE_CMD)
            last_cmd = current;
        current = current->next;
    }
    
    return last_cmd;
}

/*
Determines if variable expansion is allowed in current context.
- Returns 1 if no quotes or in double quotes.
- Returns 0 if in single quotes.
Works with process_char() for expansion handling.

Example: For input "echo '$USER'"
- When inside single quotes, returns 0 (no expansion)
- For "echo "$USER"", returns 1 (expansion allowed)
*/
int	handle_expand(t_vars *vars)
{
    if (vars->quote_depth == 0)
        return (1);
    if (vars->quote_ctx[vars->quote_depth - 1].type == '"')
        return (1);
    return (0);
}

/*
Processes special characters like dollar sign.
- Handles expansion variables if not in single quotes.
- Updates position and token boundaries.
- Creates tokens for expanded variables.
Returns:
1 if special char was processed, 0 otherwise.
Works with process_char() during tokenization.

Example: For input "echo $USER"
- Detects $ at position
- Extracts USER as variable name
- Replaces with expanded value
*/
int	process_special_char(char *input, int *i, t_vars *vars)
{
    char	*content;

    if (input[*i] == '$' && handle_expand(vars))
    {
        content = handle_expansion(input, i, vars);
        if (content)
        {
            maketoken(content, vars);
            ft_safefree((void **)&content);
            return (1);
        }
    }
    return (0);
}

/*
Processes expansion characters in the input.
- Handles variable expansion with $.
- Updates position after expansion.
- Extracts variable name and its value.
Returns:
1 if expansion was processed, 0 otherwise.
Works with process_char() during tokenization.

Example: For input "echo $HOME"
- Detects $ at position
- Creates expansion token for HOME
- Updates position past the variable name
*/
int	process_expand_char(char *input, int *i, t_vars *vars)
{
    if (input[*i] == '$')
    {
        if (process_special_char(input, i, vars))
            return (1);
        (*i)++;
        return (1);
    }
    return (0);
}

/*
Processes quoted characters in the input string.
- Handles single and double quotes.
- Updates position after quoted content.
- Creates tokens for quote content.
Returns:
1 if quote was processed, 0 otherwise.
Works with process_char() for quoted content handling.

Example: For input with quotes like "echo 'hello'"
- Processes the quoted content
- Creates appropriate token
- Returns 1 to indicate quote was handled
*/
int process_quote_char(char *input, int *i, t_vars *vars)
{
    char	*content;

    if (input[*i] == '\'' || input[*i] == '\"')
    {
        handle_quotes(input, i, vars);
        if (vars->quote_depth > 0)
        {
            content = read_quoted_content(input, i,
                    vars->quote_ctx[vars->quote_depth - 1].type);
            if (content)
            {
                maketoken(content, vars);
                ft_safefree((void **)&content);
            }
        }
        return (1);
    }
    return (0);
}

/*
Processes operator characters in the input string.
- Identifies redirection operators and pipe.
- Creates tokens for these operators.
- Updates position past the operator.
Returns:
1 if operator was processed, 0 otherwise.
Works with process_char() for operator handling.

Example: For input with operator like "cmd > file"
- Processes the '>' operator
- Creates redirect token
- Returns 1 to indicate operator was handled
*/
int process_operator_char(char *input, int *i, t_vars *vars)
{
    if (is_redirection(classify(input, *i)))
    {
        handle_redirection(input, i, vars);
        return (1);
    }
    return (0);
}

/*
Processes a single character in the input string.
- Updates token boundary at whitespace.
- Delegates to specialized handlers for:
  - Quotes
  - Variable expansion
  - Operators
- Advances position for regular characters.
Works with tokenize() for character-by-character processing.

Example: For input "echo hello"
- Sets token boundary at start of "echo"
- Advances through characters
- Handles special characters appropriately
*/
void process_char(char *input, int *i, t_vars *vars)
{
    if (!input || !i || !vars)
        return ;
    if (*i == 0 || ft_isspace(input[*i - 1]))
        vars->start = *i;
    if (process_quote_char(input, i, vars))
        return ;
    if (process_expand_char(input, i, vars))
        return ;
    if (process_operator_char(input, i, vars))
        return ;
    (*i)++;
}

/*
Processes token position for quoted content.
- Advances position past opening quote.
- Searches for matching closing quote.
- Updates position accordingly.
Returns:
Position after scanning for quote.
Works with handle_quote_token() during quote processing.

Example: For input "echo 'hello world'"
- Advances past opening quote
- Scans for closing quote
- Returns position of closing quote or end of string
*/
int scan_quote_position(char *str, int *pos, char quote_char)
{
    int start;

    start = *pos;
    (*pos)++;
    while (str[*pos])
    {
        if (str[*pos] == quote_char)
            break ;
        (*pos)++;
    }
    return start;
}

/*
Creates token for quoted content.
- Updates token boundary for quote content.
- Sets token type based on quote type.
- Creates token with the quoted content.
- Updates position for next token.
Works with handle_quote_token() for token creation.

Example: For input "echo "hello world""
- Creates token with TYPE_DOUBLE_QUOTE
- Sets content to "hello world"
- Updates position past closing quote
*/
/*
Creates token for quoted content.
- Updates token boundary for quote content.
- Sets token type based on quote type.
- Creates token with the quoted content.
- Updates position for next token.
Works with handle_quote_token() for token creation.
*/
void	create_quote_token(char *str, t_vars *vars, int *pos, int start)
{
    char	quote_char;

    quote_char = str[start];
    (*pos)++;
    vars->start = start;
    
    if (quote_char == '"')
        vars->curr_type = TYPE_DOUBLE_QUOTE;
    else
        vars->curr_type = TYPE_SINGLE_QUOTE;
    maketoken(str, vars);
    vars->start = *pos;
}

/*
Handles quoted tokens during tokenization.
- Detects quote character type (single/double).
- Processes content between quotes.
- Updates position past closing quote.
- Creates token with appropriate type.
Works with process_char() for quoted content.

Example: For input "echo "hello world""
- Detects quote at start position
- Extracts "hello world" as content
- Creates token for quoted content
- Updates position past closing quote
*/
void	handle_quote_token(char *str, t_vars *vars, int *pos)
{
    char       quote_char;
    int        start;

    if (!str || !vars || !pos)
        return ;
    quote_char = str[*pos];
    start = scan_quote_position(str, pos, quote_char);
    if (str[*pos] == quote_char)
    {
        create_quote_token(str, vars, pos, start);
    }
    else
    {
        vars->quote_depth++;
        vars->quote_ctx[vars->quote_depth - 1].type = quote_char;
    }
}

/*
Processes a single character in the input string.
- Handles quoted content and expansion variables.
- Updates token boundaries and position.
- Creates tokens when token boundaries are reached.
Works with tokenize() for character-by-character processing.

Example: For input "echo hello"
- Sets token boundary at start of "echo"
- Advances through characters until space
- Then creates token for "echo"
OLD VERSION
void	process_char(char *input, int *i, t_vars *vars)
{
    char	*content;

    if (!input || !i || !vars)
        return ;
    if (*i == 0 || ft_isspace(input[*i - 1]))
        vars->start = *i;
    if (input[*i] == '\'' || input[*i] == '\"')
    {
        handle_quotes(input, i, vars);
        if (vars->quote_depth > 0)
        {
            content = read_quoted_content(input, i,
                    vars->quote_ctx[vars->quote_depth - 1].type);
            if (content)
            {
                maketoken(content, vars);
                ft_safefree((void **)&content);
            }
        }
        return ;
    }
    if (process_expand_char(input, i, vars))
        return ;
    if (is_redirection(classify(input, *i)))
    {
        handle_redirection(input, i, vars);
        return ;
    }
    (*i)++;
}
*/

/*
Processes redirection operators in the input.
- Handles <, >, << and >> operators.
- Creates tokens for the operators.
- Updates position past the operator.
Works with process_char() for operator handling.

Example: For input "cat > file.txt"
- When position is at '>'
- Creates redirect token
- Updates position past '>'
*/
void	handle_redirection(char *input, int *i, t_vars *vars)
{
    int	pos;
    int	result;

    pos = *i;
    result = operators(input, pos, vars->start, vars);
    *i = result;
}

/*
Main tokenization function for input processing.
- Processes input character by character.
- Creates tokens for commands, operators, etc.
- Handles quotes and variable expansion.
- Updates token list in vars structure.
Works with lexerlist() for initial token creation.

Example: For input "echo hello | grep world"
- Processes "echo hello" as command token
- Processes "|" as pipe token
- Processes "grep world" as command token
- Builds linked list of these tokens
*/
void	tokenize(char *input, t_vars *vars)
{
    int	i;

    i = 0;
    vars->start = 0;
    while (input[i])
        process_char(input, &i, vars);
}

/*
Process non-command tokens from input string.
- Creates token of appropriate type.
- Handles memory allocation and cleanup.
- Adds token to the token list.
Works with maketoken() for token creation.

Example: For pipe operator "|"
- Creates token with TYPE_PIPE
- Adds to token list
- Updates current pointer
OLD VERSION
void	process_other_token(char *input, t_vars *vars)
{
    char	*token;
    t_node	*node;

    if (vars->pos <= vars->start)
        return ;
    token = ft_substr(input, vars->start, vars->pos - vars->start);
    if (!token)
        return ;
    if (vars->curr_type == TYPE_ARGS)
        node = new_cmd_node(token);
    else
        node = new_other_node(token, vars->curr_type);
    ft_safefree((void **)&token);
    if (!node)
        return ;
    build_token_linklist(vars, node);
}
*/
/*
Processes a non-command token during tokenization.
- Creates a new node with specified token type.
- For argument tokens, appends to the previous command.
- For other tokens, adds as a new node in the list.
Works with maketoken().
OLD VERSION
void process_other_token(char *token, t_vars *vars)
{
    int created_node;
    
    created_node = 0;
    
    // If it's an argument, append it to the previous command
    if (vars->curr_type == TYPE_ARGS && vars->current && 
        vars->current->type == TYPE_CMD)
    {
        fprintf(stderr, "DEBUG: Appending argument '%s' to command\n", token);
        append_arg(vars->current, token);
    }
    // Otherwise create a new node
    else
    {
        // Create the token node
        created_node = makenode(vars, token);
        
        // Initialize linked list if this is the first token
        if (created_node && !vars->head)
            vars->head = vars->current;
    }
    
    fprintf(stderr, "DEBUG: Token processed: type=%d, value=%s\n", 
            vars->curr_type, token);
}
*/
void process_other_token(char *token, t_vars *vars)
{
    t_node *node = NULL;
    
    fprintf(stderr, "DEBUG: process_other_token called with token='%s', type=%d\n", 
            token, vars->curr_type);
            
    if (!token || !vars)
        return;
    
    // For pipe tokens, ensure proper type is set
    if (strcmp(token, "|") == 0)
    {
        fprintf(stderr, "DEBUG: Forcing pipe token type to TYPE_PIPE\n");
        vars->curr_type = TYPE_PIPE;
    }
    
    // Create the appropriate node
    node = new_other_node(token, vars->curr_type);
    if (!node)
    {
        fprintf(stderr, "DEBUG: Failed to create node for token '%s'\n", token);
        return;
    }
    
    // Add to token list
    if (!vars->head)
    {
        vars->head = node;
        vars->current = node;
        fprintf(stderr, "DEBUG: Set head token: type=%d, value='%s'\n", 
                node->type, token);
    }
    else
    {
        vars->current->next = node;
        node->prev = vars->current;
        vars->current = node;
        fprintf(stderr, "DEBUG: Added token to list: type=%d, value='%s'\n", 
                node->type, token);
    }
    
    fprintf(stderr, "DEBUG: Other token node created: type=%d\n", node->type);
}

/*
Creates a new command node from a token.
- Allocates node with command type.
- Sets up args array with initial token.
- Handles memory errors properly.
Returns:
Pointer to new command node or NULL on failure.
Works with process_cmd_token() for command creation.

Example: For command "echo"
- Creates node with TYPE_CMD type
- Sets args[0] to "echo"
- Returns node pointer
*/
t_node	*make_cmdnode(char *token)
{
    t_node	*node;

    node = initnode(TYPE_CMD, token);
    if (!node)
        return (NULL);
    node->args = malloc(sizeof(char *) * 2);
    if (!node->args)
    {
        ft_safefree((void **)&node);
        return (NULL);
    }
    node->args[0] = ft_strdup(token);
    if (!node->args[0])
    {
        ft_safefree((void **)&node->args);
        ft_safefree((void **)&node);
        return (NULL);
    }
    node->args[1] = NULL;
    return (node);
}

/*
Creates a new command node with proper setup.
- Wraps make_cmdnode with error handling.
- Ensures memory cleanup on failure.
Returns:
- Pointer to new command node or NULL on failure.
- Works with process_cmd_token() for command creation.

Example: For command token "grep"
- Creates new command node
- Handles memory errors
- Returns node or NULL on failure
*/
t_node	*new_cmd_node(char *token)
{
    t_node	*node;

    node = make_cmdnode(token);
    if (!node)
    {
        ft_safefree((void **)&token);
        return (NULL);
    }
    return (node);
}

/*
Creates a non-command token node.
- Allocates node with specified token type.
- Handles memory errors properly.
Returns:
Pointer to new token node or NULL on failure.
Works with process_other_token() for token creation.

Example: For redirect token ">"
- Creates node with TYPE_OUT_REDIRECT
- Returns node pointer or NULL on failure
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
Updates the token list with a new node.
- Handles first token as head.
- Otherwise adds to end of list.
- Updates current pointer.
Works with process_cmd_token() and process_other_token().

Example: When adding command node
- If first token, sets as head
- Otherwise links to previous token
- Updates current pointer
*/
void build_token_linklist(t_vars *vars, t_node *node)
{
    if (!vars->head)
    {
        vars->head = node;
        vars->current = node;
    }
    else
    {
        vars->current->next = node;
        node->prev = vars->current;
        vars->current = node;
    }
}

/*
Process command token by splitting input string.
- Splits input by whitespace.
- Processes quotes in arguments.
- Creates command node with arguments.
- Adds node to token list.
Works with maketoken() for command token creation.

Example: For input "echo hello world"
- Splits into ["echo", "hello", "world"]
- Creates command node with these args
- Adds to token list
*/
void process_cmd_token(char *input, t_vars *vars)
{
    char	**args;
    t_node	*node;
    int		i;

    args = ft_splitstr(input, " \t\n\v\f\r");
    if (!args)
        return ;
    i = 0;
    while (args[i])
    {
        process_quotes_in_arg(&args[i]);
        if (is_flag_arg(args, i))
        {
            join_flag_args(args, i);
        }
        i++;
    }
    if (args[0])
    {
        node = build_cmdarg_node(args);
        if (node)
            build_token_linklist(vars, node);
    }
    ft_free_2d(args, ft_arrlen(args));
}

/*
Checks if current argument is a flag separator.
- Identifies standalone '-' followed by alphabetic arg.
- Used to detect cases like "cmd - x" that should be "cmd -x".
Returns:
- 1 if it's a flag arg pattern.
- 0 if not.
Works with process_cmd_token() for argument processing.

Example: For arguments ["ls", "-", "l"]
- Returns 1 for position 1
- Indicates "-" and "l" should be joined as "-l"
*/
int is_flag_arg(char **args, int i)
{
    if (i > 0 && args[i][0] == '-' && ft_strlen(args[i]) == 1 
        && args[i+1] && ft_isalpha(args[i+1][0]))
    {
        return (1);
    }
    return (0);
}

/*
Joins flag arguments that were incorrectly split.
- Combines "-" and the following argument.
- Shifts remaining arguments up.
- Updates the args array in place.
Works with process_cmd_token() during argument processing.

Example: For arguments ["ls", "-", "l", "a"]
- Joins "-" and "l" to become "-l"
- Shifts "a" up in position
- Results in ["ls", "-l", "a", NULL]
*/
void join_flag_args(char **args, int i)
{
    char	*combined;
    int		j;

    combined = ft_strjoin(args[i], args[i+1]);
    if (combined)
    {
        ft_safefree((void **)&args[i]);
        ft_safefree((void **)&args[i+1]);
        args[i] = combined;
        
        j = i + 1;
        while (args[j+1])
        {
            args[j] = args[j+1];
            j++;
        }
        args[j] = NULL;
    }
}

/*
Creates command node from parsed arguments.
- Uses first arg as command name.
- Adds remaining args to node's args array.
- Handles memory allocation properly.
Returns:
- New command node or NULL on failure.
Works with process_cmd_token() during command creation.

Example: For arguments ["grep", "pattern", "file"]
- Creates command node with "grep"
- Adds "pattern" and "file" as arguments
- Returns the complete command node
*/
t_node *build_cmdarg_node(char **args)
{
    t_node	*node;
    int		i;
     
    if (!args || !args[0])
        return (NULL);
    node = new_cmd_node(args[0]);
    if (!node)
        return (NULL);
    
    i = 1;
    while (args[i])
    {
        append_arg(node, args[i]);
        i++;
    }
    return (node);
}

/*
Processes arguments by removing outer quotes.
- Applies quote removal to each argument.
- Ensures proper argument interpretation.
Works with process_cmd_token() for argument processing.

Example: For arguments ["'hello'", "\"world\""]
- Processes to remove outer quotes
- Transforms to ["hello", "world"]
*/
void process_args_tokens(char **args)
{
    int	i;

    i = 0;
    while (args[i])
    {
        process_quotes_in_arg(&args[i]);
        i++;
    }
}

/*
Handles quoted tokens during tokenization.
- Detects quote character type (single/double).
- Processes content between quotes.
- Updates position past closing quote.
- Creates token with appropriate type.
Works with process_char() for quoted content.

Example: For input "echo "hello world""
- Detects quote at start position
- Extracts "hello world" as content
- Creates token for quoted content
- Updates position past closing quote
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



