/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 16:36:32 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 04:01:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Collects and stores command nodes from the token list in vars->cmd_nodes.
- Traverses linked list of tokens starting from vars->head.
- Identifies command nodes (TYPE_CMD).
- Stores them in vars->cmd_nodes array for easier reference.
- Updates vars->cmd_count with total count of commands found.
Works with proc_token_list().
*/
void	get_cmd_nodes(t_vars *vars)
{
    t_node	*current;

    if (!vars || !vars->head)
        return ;
    vars->cmd_count = 0;
    current = vars->head;
    while (current)
    {
        if (current->type == TYPE_CMD && vars->cmd_count < 100)
        {
            vars->cmd_nodes[vars->cmd_count] = current;
            vars->cmd_count++;
            fprintf(stderr, "DEBUG: Added command node: '%s'\n",
                current->args[0]);
        }
        current = current->next;
    }
}

/*
Locates the next command node in the token list after a specified position.
- Traverses the token list starting from the given node.
- Returns first node of type TYPE_CMD encountered.
Returns:
- Pointer to the next command node.
- NULL if no command node found or if start is NULL.
Works with proc_pipes_pt1() and proc_pipes_pt2().
*/
t_node	*find_next_cmd(t_node *start)
{
    t_node	*next_cmd;

    if (!start)
        return (NULL);
    next_cmd = start;
    while (next_cmd && next_cmd->type != TYPE_CMD)
        next_cmd = next_cmd->next;
    return (next_cmd);
}

/*
Initializes the first pipe node in a pipeline structure.
- Sets left child to previous command node.
- Sets right child to next command node.
- Both commands must be valid for pipe to be properly configured.
Works with proc_pipes_pt1().
*/
void	setup_first_pipe(t_node *pipe_node, t_node *prev_cmd, t_node *next_cmd)
{
    if (!pipe_node || !prev_cmd || !next_cmd)
        return ;
    pipe_node->left = prev_cmd;
    pipe_node->right = next_cmd;
    fprintf(stderr, "DEBUG: Setting pipe root with left: %s\n",
        prev_cmd->args[0]);
}

/*
Connects additional pipe nodes in a pipeline chain.
- Sets pipe_node's left child to last_cmd.
- Sets pipe_node's right child to next_cmd.
- Links the current pipe to the previous pipe chain.
- Updates the pipeline structure for continued processing.
Works with proc_pipes_pt2().
*/
void	setup_next_pipes(t_node *pipe_node, t_node *last_pipe,
    t_node *last_cmd, t_node *next_cmd)
{
    if (!pipe_node || !last_pipe || !last_cmd || !next_cmd)
        return ;
    pipe_node->left = last_cmd;
    pipe_node->right = next_cmd;
    last_pipe->right = pipe_node;
    fprintf(stderr, "DEBUG: Added additional pipe with left: %s\n",
        last_cmd->args[0]);
}

/*
Processes the first pipe node in the token list.
- Identifies the first pipe token and its surrounding commands.
- Initializes the pipe node with left and right command references.
- Updates tracking pointers for subsequent pipe processing.
Returns:
- Pointer to root pipe node if found and initialized.
- NULL if no valid pipe configuration found.
Works with proc_token_list().
*/
t_node	*proc_pipes_pt1(t_vars *vars, t_node **last_pipe, t_node **last_cmd)
{
    t_node	*current;
    t_node	*pipe_root;
    t_node	*next_cmd;

    current = vars->head;
    pipe_root = NULL;
    *last_pipe = NULL;
    *last_cmd = NULL;
    while (current)
    {
        if (current->type == TYPE_CMD)
            *last_cmd = current;
        else if (current->type == TYPE_PIPE && !pipe_root
            && current->prev && current->prev->type == TYPE_CMD)
        {
            pipe_root = current;
            next_cmd = find_next_cmd(current->next);
            if (next_cmd)
                setup_first_pipe(pipe_root, current->prev, next_cmd);
            *last_pipe = pipe_root;
            *last_cmd = next_cmd;
        }
        current = current->next;
    }
    return (pipe_root);
}

/*
Processes additional pipe nodes after the first one.
- Traverses token list to find remaining pipe tokens.
- Connects each pipe to the growing pipeline structure.
- Updates tracking pointers for each new pipe addition.
- Maintains the pipeline chain for AST construction.
Works with proc_token_list().
*/
void	proc_pipes_pt2(t_vars *vars, t_node *pipe_root,
    t_node **last_pipe, t_node **last_cmd)
{
    t_node	*current;
    t_node	*next_cmd;

    current = vars->head;
    while (current)
    {
        if (current->type == TYPE_PIPE && pipe_root
            && current != pipe_root)
        {
            next_cmd = find_next_cmd(current->next);
            if (*last_cmd && next_cmd)
            {
                setup_next_pipes(current, *last_pipe,
                    *last_cmd, next_cmd);
                *last_pipe = current;
                *last_cmd = next_cmd;
            }
        }
        current = current->next;
    }
}

/*
Configures a redirection node with source and target commands.
- Sets left child to source command node.
- Sets right child to target command/filename node.
- Establishes the redirection relationship in the AST.
Works with proc_redir_pt1() and proc_redir_pt2().
*/
void	setup_redir_ast(t_node *redir, t_node *cmd, t_node *target)
{
    if (!redir || !cmd || !target)
        return ;
    redir->left = cmd;
    redir->right = target;
    fprintf(stderr, "DEBUG: Created redirection from %s to %s\n",
        cmd->args[0], target->args[0]);
}

/*
Updates pipe structure when commands are redirected.
- Traverses pipe chain looking for references to the command.
- Replaces command references with redirection node references.
- Preserves pipe structure while incorporating redirections.
- Handles both left and right side command replacements.
Works with proc_redir_pt2().
*/
void	upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir)
{
    t_node	*pipe_node;

    if (!pipe_root || !cmd || !redir)
        return ;
    pipe_node = pipe_root;
    while (pipe_node)
    {
        if (pipe_node->left == cmd)
            pipe_node->left = redir;
        else if (pipe_node->right == cmd)
            pipe_node->right = redir;
        pipe_node = pipe_node->right;
        if (pipe_node && pipe_node->type != TYPE_PIPE)
            break ;
    }
}

/*
Checks if a token is a redirection operator.
- Compares token type against all redirection types.
Returns:
- 1 if token is a redirection operator.
- 0 otherwise.
Works with is_valid_redir_node().
*/
int	is_redir_token(t_tokentype type)
{
    return (type == TYPE_OUT_REDIRECT
        || type == TYPE_APPEND_REDIRECT
        || type == TYPE_IN_REDIRECT
        || type == TYPE_HEREDOC);
}

/*
Determines if a redirection node has valid adjacent commands.
- Checks if next node exists and is a command.
Returns:
- 1 if redirection has valid syntax.
- 0 otherwise.
Works with proc_redir_pt1().
*/
int	is_valid_redir_node(t_node *current)
{
    if (!current)
        return (0);
    if (!is_redir_token(current->type))
        return (0);
    if (!current->next || current->next->type != TYPE_CMD)
        return (0);
    return (1);
}

/*
Gets target command for redirection.
- Uses previous command if it exists and is a command node.
- Otherwise uses last tracked command.
Returns:
- Pointer to target command node.
- NULL if no suitable command found.
Works with proc_redir_pt1().
*/
t_node	*get_redir_target(t_node *current, t_node *last_cmd)
{
    t_node	*target;

    target = NULL;
    if (current->prev && current->prev->type == TYPE_CMD)
        target = current->prev;
    else
        target = last_cmd;
    return (target);
}

/*
Processes the first part of redirection nodes identification.
- Traverses token list to find redirection operators.
- Records last command seen for reference.
- Identifies redirection targets.
Returns:
- First valid redirection node if no pipe root exists.
- NULL otherwise or if no valid redirections found.
Works with proc_token_list().
*/
t_node	*proc_redir_pt1(t_vars *vars, t_node *pipe_root)
{
    t_node	*current;
    t_node	*last_cmd;
    t_node	*redir_root;
    t_node	*target_cmd;

    current = vars->head;
    last_cmd = NULL;
    redir_root = NULL;
    while (current)
    {
        if (current->type == TYPE_CMD)
            last_cmd = current;
        else if (is_valid_redir_node(current))
        {
            target_cmd = get_redir_target(current, last_cmd);
            if (target_cmd)
            {
                setup_redir_ast(current, target_cmd, current->next);
                if (!pipe_root && !redir_root)
                    redir_root = current;
            }
        }
        current = current->next;
    }
    return (redir_root);
}

/*
Processes the second part of redirection nodes handling.
- Updates pipe structure with redirection references.
- Links redirections into existing pipe hierarchy.
- Only modifies pipe structure if redirection affects piped commands.
Works with proc_token_list().
*/
void	proc_redir_pt2(t_vars *vars, t_node *pipe_root)
{
    t_node	*current;
    t_node	*target_cmd;
    t_node	*last_cmd;

    current = vars->head;
    last_cmd = NULL;
    while (current)
    {
        if (current->type == TYPE_CMD)
            last_cmd = current;
        else if (is_valid_redir_node(current))
        {
            target_cmd = get_redir_target(current, last_cmd);
            if (target_cmd && pipe_root)
                upd_pipe_redir(pipe_root, target_cmd, current);
        }
        current = current->next;
    }
}

/*
Processes the entire token list to build the AST structure.
- Identifies command nodes for reference.
- Processes pipe tokens to build pipeline structure.
- Processes redirection tokens and integrates with pipe structure.
- Determines appropriate root node for the final AST.
Returns:
- Pointer to root node of the constructed AST.
- NULL if invalid syntax or no commands found.
Works with build_ast().

Example: For input "ls -l | grep a > output.txt":
- First identifies command nodes "ls" and "grep"
- Processes pipe to connect them
- Processes redirection to output.txt
- Returns pipe node as root, with redirection integrated
*/
t_node	*proc_token_list(t_vars *vars)
{
    t_node	*pipe_root;
    t_node	*last_pipe;
    t_node	*last_cmd;
    t_node	*redir_root;

    if (!vars || !vars->head)
    {
        fprintf(stderr, "DEBUG: No tokens to process\n");
        return (NULL);
    }
    get_cmd_nodes(vars);
    fprintf(stderr, "DEBUG: Processing token list for AST\n");
    pipe_root = proc_pipes_pt1(vars, &last_pipe, &last_cmd);
    proc_pipes_pt2(vars, pipe_root, &last_pipe, &last_cmd);
    redir_root = proc_redir_pt1(vars, pipe_root);
    proc_redir_pt2(vars, pipe_root);
    if (pipe_root)
        return (pipe_root);
    else if (redir_root)
        return (redir_root);
    else if (vars->cmd_count > 0)
        return (vars->cmd_nodes[0]);
    return (NULL);
}

/*
Determines the appropriate root node for the AST.
- Prioritizes pipe nodes as root if available.
- Falls back to first command node if no pipes exist.
Returns:
- Pointer to the most suitable root node.
- NULL if no valid nodes exist.
Works with build_ast().
*/
t_node	*set_ast_root(t_node *pipe_node, t_vars *vars)
{
    if (pipe_node)
    {
        fprintf(stderr, "DEBUG: Built AST successfully with root type %d\n",
            pipe_node->type);
        return (pipe_node);
    }
    if (vars->cmd_count > 0)
    {
        fprintf(stderr, "DEBUG: Built AST with single command\n");
        return (vars->cmd_nodes[0]);
    }
    fprintf(stderr, "DEBUG: No valid nodes found for AST\n");
    return (NULL);
}

/*
Detects if a pipe token appears at the beginning of input.
- Checks if the first token is a pipe (syntax error).
- Sets error code and outputs error message if detected.
Returns:
- 1 if pipe found at beginning (error condition).
- 0 if no error detected or no tokens exist.
Works with chk_pipe_syntax_err().
*/
int	chk_start_pipe(t_vars *vars)
{
    if (!vars || !vars->head)
        return (0);
    if (vars->head->type == TYPE_PIPE)
    {
        fprintf(stderr, "DEBUG: Error: Pipe at start of input\n");
        ft_putstr_fd("bleshell: unexpected syntax error at '|'\n", 2);
        vars->error_code = 258;
        return (1);
    }
    return (0);
}

/*
Detects multiple pipe tokens in sequence.
- Tracks count of consecutive pipe tokens.
- Reports syntax error if multiple pipes are detected.
- Sets error code and outputs error message.
Returns:
- 1 if consecutive pipes detected (error).
- 0 if no consecutive pipes.
Works with chk_next_pipes().
*/
int	detect_multi_pipes(t_vars *vars, int pipes_count)
{
    if (pipes_count > 1)
    {
        fprintf(stderr, "DEBUG: Error: Multiple consecutive pipes\n");
        ft_putstr_fd("bleshell: syntax error near unexpected token '|'\n", 2);
        vars->error_code = 258;
        return (1);
    }
    return (0);
}

/*
Detects adjacent pipe tokens in the list.
- Checks if current and next token are both pipes.
- Reports syntax error if adjacent pipes are detected.
- Sets error code and outputs error message.
Returns:
- 1 if adjacent pipes detected (error).
- 0 if no adjacent pipes.
Works with chk_next_pipes().
*/
int	detect_adj_pipes(t_vars *vars, t_node *current)
{
    if (current->next && current->next->type == TYPE_PIPE)
    {
        fprintf(stderr, "DEBUG: Error: Adjacent pipe tokens\n");
        ft_putstr_fd("bleshell: syntax error near unexpected token '|'\n", 2);
        vars->error_code = 258;
        return (1);
    }
    return (0);
}

/*
Checks for consecutive pipe tokens in the token list.
- Tracks sequence of pipe tokens.
- Reports syntax error if multiple pipes found consecutively.
Returns:
- 1 if pipe syntax error found.
- 0 if no errors detected.
Works with chk_pipe_syntax_err().
*/
int	chk_next_pipes(t_vars *vars)
{
    t_node	*current;
    int		multi_pipes;
    int		error;

    current = vars->head;
    multi_pipes = 0;
    while (current)
    {
        if (current->type == TYPE_PIPE)
        {
            multi_pipes++;
            error = detect_multi_pipes(vars, multi_pipes);
            if (error)
                return (error);
            error = detect_adj_pipes(vars, current);
            if (error)
                return (error);
        }
        else
            multi_pipes = 0;
        current = current->next;
    }
    return (0);
}

/*
Detects if a pipe token appears at the end of input.
- Checks if the last token is a pipe.
- Indicates more input is needed to complete command.
Returns:
- 2 if pipe found at end (requires more input).
- 0 if no pipe at end or no tokens exist.
Works with chk_pipe_syntax_err().
*/
int	chk_end_pipe(t_vars *vars)
{
    t_node	*current;

    current = vars->head;
    while (current && current->next)
        current = current->next;
    if (current && current->type == TYPE_PIPE)
    {
        fprintf(stderr, "DEBUG: Pipe at end of input, need more input\n");
        return (2);
    }
    return (0);
}

/*
Performs comprehensive check of pipe syntax in token list.
- Detects pipes at beginning (error).
- Detects consecutive pipes (error).
- Detects pipes at end (needs more input).
Returns:
- 0 if pipe syntax is valid.
- 1 if syntax error detected.
- 2 if more input needed (pipe at end).
Works with handle_incomplete_pipe().

Example: For input "| ls":
- Returns 1 (error: pipe at beginning)
For input "ls | grep a |":
- Returns 2 (needs more input)
*/
int	chk_pipe_syntax_err(t_vars *vars)
{
    int	result;

    if (!vars->head)
    {
        fprintf(stderr, "DEBUG: No tokens to check pipe syntax\n");
        return (0);
    }
    result = chk_start_pipe(vars);
    if (result != 0)
        return (result);
    result = chk_next_pipes(vars);
    if (result != 0)
        return (result);
    result = chk_end_pipe(vars);
    if (result != 0)
        return (result);
    return (0);
}

/*
Combines original input with new input line.
- Joins strings with a space separator.
- Properly handles memory for both inputs.
Returns:
- Newly allocated string containing joined input.
- NULL on memory allocation failure.
Works with handle_trailing_pipe_pt2().
*/
char	*merge_input(char *input, char *line)
{
    char	*temp;
    char	*new_input;

    temp = ft_strjoin(input, " ");
    if (!temp)
    {
        ft_safefree((void **)&line);
        return (NULL);
    }
    new_input = ft_strjoin(temp, line);
    ft_safefree((void **)&temp);
    ft_safefree((void **)&line);
    if (!new_input)
        return (NULL);
    return (new_input);
}

/*
Prepares for handling an incomplete command ending with pipe.
- Cleans up existing token list.
- Creates a copy of current input for subsequent processing.
Returns:
- Copy of the input string for further processing.
- NULL on memory allocation failure.
Works with handle_incomplete_pipe().
*/
char	*handle_trailing_pipe_pt1(char *input, t_vars *vars)
{
    char	*new_input;

    fprintf(stderr, "DEBUG: Processing unfinished pipe\n");
    cleanup_token_list(vars);
    new_input = ft_strdup(input);
    if (!new_input)
        return (NULL);
    return (new_input);
}

/*
Gets additional input for command with trailing pipe.
- Prompts user for more input.
- Adds input to history if not empty.
- Joins with previous input and builds new token list.
Returns:
- Updated input string with new content.
- NULL on failure or EOF.
Works with handle_incomplete_pipe().
*/
char	*handle_trailing_pipe_pt2(char *new_input, t_vars *vars)
{
    char	*line;
    char	*joined_input;

    line = readline("COMMAND> ");
    if (!line)
    {
        ft_safefree((void **)&new_input);
        return (NULL);
    }
    if (*line)
        add_history(line);
    joined_input = merge_input(new_input, line);
    ft_safefree((void **)&new_input);
    if (!joined_input)
        return (NULL);
    cleanup_token_list(vars);
    tokenize(joined_input, vars);
    lexerlist(joined_input, vars);
    return (joined_input);
}

/*
Handles commands with trailing pipes by prompting for more input.
- Continues prompting until command is syntactically complete.
- Repeatedly tokenizes and checks syntax of expanded input.
Returns:
- Complete command string with all input parts joined.
- NULL on memory allocation failure or EOF.
Works with build_ast().

Example: For input "ls |":
- Prompts user with "COMMAND>"
- User types "grep hello"
- Returns "ls | grep hello"
*/
char	*handle_incomplete_pipe(char *input, t_vars *vars)
{
    char	*new_input;
    int		continue_prompting;

    new_input = handle_trailing_pipe_pt1(input, vars);
    if (!new_input)
        return (NULL);
    continue_prompting = 1;
    while (continue_prompting)
    {
        new_input = handle_trailing_pipe_pt2(new_input, vars);
        if (!new_input)
            return (NULL);
        if (chk_pipe_syntax_err(vars) != 2)
            continue_prompting = 0;
    }
    fprintf(stderr, "DEBUG: Completed command: '%s'\n", new_input);
    return (new_input);
}

/*
Constructs Abstract Syntax Tree from token list.
- Processes tokens to identify commands, pipes, redirections.
- Creates hierarchical structure for command execution.
- Sets vars->astroot for later reference.
Returns:
- Pointer to root node of constructed AST.
- NULL if no valid syntax or tokens found.
Works with process_command() in main execution loop.

Example: For input "ls -l | grep foo > output.txt":
- Builds tree with pipe as root
- Left child is "ls -l" command
- Right child is redirection node
- Redirection's left child is "grep foo"
- Redirection's right child is "output.txt"
*/
t_node	*build_ast(t_vars *vars)
{
    t_node	*root;

    fprintf(stderr, "DEBUG: Building AST for commands and redirections\n");
    if (!vars || !vars->head)
    {
        fprintf(stderr, "DEBUG: No tokens (vars->head is NULL)!\n");
        return (NULL);
    }
    root = proc_token_list(vars);
    if (!root && vars->cmd_count > 0)
    {
        root = vars->cmd_nodes[0];
        fprintf(stderr, "DEBUG: Using first command as root: %s\n",
            root->args ? root->args[0] : "NULL");
    }
    vars->astroot = root;
    return (root);
}

/*
Prints detailed information about a token for debugging.
- Shows token type, string representation, and arguments.
- Formats output for readability in debug logs.
Works with debug_print_token_list().
*/
void	debug_print_token_attrib(t_node *current, int i)
{
    int	j;

    fprintf(stderr, "Token %d: Type=%d (%s), Value='%s', Args=[",
        i, current->type, get_token_str(current->type),
        current->args ? current->args[0] : "NULL");
    if (current->args)
    {
        j = 0;
        while (current->args[j])
        {
            fprintf(stderr, "'%s'", current->args[j]);
            if (current->args[j + 1])
                fprintf(stderr, ", ");
            j++;
        }
    }
    fprintf(stderr, "]\n");
}

/*
Prints complete token list for debugging purposes.
- Outputs each token with its attributes.
- Shows token indices for reference.
- Formats output with clear start and end markers.
Works with build_ast() for debugging.
*/
void	debug_print_token_list(t_vars *vars)
{
    t_node	*current;
    int		i;

    if (!vars || !vars->head)
        return ;
    fprintf(stderr, "\n=== TOKEN LIST ===\n");
    current = vars->head;
    i = 0;
    while (current)
    {
        debug_print_token_attrib(current, i);
        i++;
        current = current->next;
    }
    fprintf(stderr, "=== END TOKEN LIST ===\n\n");
}
