/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 11:31:02 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 06:19:27 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Reads input line from the user with prompt display.
- Displays the shell prompt and awaits user input.
- Handles Ctrl+D (EOF) by calling builtin_exit.
- Adds non-empty lines to command history.
Returns:
- User input as an allocated string.
- Never returns on EOF (exits program).
Works with main() in command loop.

Example: When user types "ls -la"
- Displays prompt "bleshell$> "
- Returns "ls -la" as an allocated string
- Adds command to history for up-arrow recall
*/
char	*reader(t_vars *vars)
{
    char	*line;

    line = readline(PROMPT);
    if (!line)
        builtin_exit(vars);
    if (*line)
        add_history(line);
    return (line);
}

/*
Sets up environment variables for the shell.
- Duplicates environment array from envp.
- Handles memory allocation errors.
- Initializes environment-dependent shell variables.
Works with init_shell().

Example: When shell starts
- Copies environment variables from system
- Sets up SHLVL variable for shell nesting level
- Reports errors if environment setup fails
*/
void	setup_env(t_vars *vars, char **envp)
{
    vars->env = dup_env(envp);
    if (!vars->env)
    {
        ft_putstr_fd("bleshell: error: Failed to duplicate environment\n", 2);
        exit(1);
    }
    vars->shell_level = get_shell_level(vars);
    if (vars->shell_level == -1)
    {
        ft_putstr_fd("bleshell: error: Failed to get shell level\n", 2);
        exit(1);
    }
    vars->shell_level = incr_shell_level(vars);
    if (!vars->shell_level)
        ft_putstr_fd("bleshell: warning: Failed to increment SHLVL\n", 2);
}

/*
Initializes the shell environment and settings.
- Zeroes all fields in vars structure.
- Sets up environment variables.
- Loads command history from history file.
- Configures signal handlers for interactive use.
Works with main() at program startup.

Example: At shell startup
- Initializes all shell state variables to default values
- Loads environment and history
- Sets up signal handling for terminal interaction
OLD VERSION
void	init_shell(t_vars *vars, char **envp)
{	
    ft_memset(vars, 0, sizeof(t_vars));
    setup_env(vars, envp);
    vars->quote_depth = 0;
    load_history();
    load_signals();
}
*/
/*
Initializes the shell environment and variables.
- Sets up signal handlers.
- Initializes environment variables.
- Sets up shell history.
- Prepares the command prompt.
Works with main() as program entry point.
*/
/*
Initializes the shell environment and variables.
- Sets up signal handlers.
- Initializes environment variables.
- Sets up shell history.
- Prepares the command prompt.
Works with main() as program entry point.
*/
void init_shell(t_vars *vars, char **envp)
{
    int result;
    
    vars->env = dup_env(envp);
    if (!vars->env)
    {
        fprintf(stderr, "ERROR: Failed to duplicate environment variables\n");
        exit(1);
    }
    
    get_shell_level(vars);
    
    fprintf(stderr, "DEBUG: About to increment shell level\n");
    result = incr_shell_level(vars);
    fprintf(stderr, "DEBUG: incr_shell_level returned %d\n", result);
    
    if (result != 0)
    {
        fprintf(stderr, "DEBUG: Showing warning about SHLVL\n");
        fprintf(stderr, "bleshell: warning: Failed to increment SHLVL\n");
    }
    else
    {
        fprintf(stderr, "DEBUG: Shell level successfully incremented to %d\n", vars->shell_level);
    }
    
    // Use load_signals instead of setup_signals
    load_signals();
    
    // Load history instead of calling init_history
    load_history();
}

/*
Processes input with quotes that need completion.
- Gets additional input for unclosed quotes.
- Re-tokenizes the completed command.
- Updates token list with completed input.
Returns:
Newly allocated complete command string.
NULL on memory allocation failure.
Works with process_command().

Example: When user types "echo "hello
- Prompts for completion of the double quote
- User types "world" and Enter
- Returns combined string: echo "hello world"
*/
char	*handle_quote_completion(char *cmd, t_vars *vars)
{
    char	*new_cmd;

    new_cmd = fix_open_quotes(cmd, vars);
    if (!new_cmd)
        return (NULL);
    if (new_cmd != cmd)
    {
        if (cmd != vars->error_msg)
            ft_safefree((void **)&cmd);
        cmd = new_cmd;
    }
    cleanup_token_list(vars);
    tokenize(cmd, vars);
    lexerlist(cmd, vars);
    return (cmd);
}

/*
Processes input with pipes that need continuation.
- Gets additional input for pipes at end of line.
- Re-tokenizes the completed command.
- Ensures proper error handling.
Returns:
Newly allocated complete command string.
NULL on memory allocation failure.
Works with process_command().

Example: When user types "ls |"
- Prompts for continuation after the pipe
- User types "grep foo" and Enter
- Returns combined string: "ls | grep foo"
*/
char	*handle_pipe_valid(char *cmd, t_vars *vars, int syntax_chk)
{
    char	*new_cmd;
	char	*pipe_cmd;

    if (syntax_chk != 2)
	{
        return (cmd);
	}
	pipe_cmd = ft_strdup(cmd);
	if (handle_unfinished_pipes(&pipe_cmd, vars, NULL) >= 0)
    	new_cmd = pipe_cmd;
	else
    	new_cmd = NULL;
    if (!new_cmd)
        return (NULL);
    if (new_cmd != cmd)
    {
        if (cmd != vars->error_msg)
            ft_safefree((void **)&cmd);
        cmd = new_cmd;
    }
    return (cmd);
}

/*
Builds and executes the command's abstract syntax tree.
- Creates AST from tokenized input.
- Executes the command if AST built successfully.
- Provides debug information about the process.
Returns:
Nothing (void function).
Works with process_command().

Example: For "echo hello | grep h"
- Builds AST with pipe node at root
- Echo command on left branch, grep on right
- Executes the pipeline with proper redirection
*/
void	build_and_execute(t_vars *vars)
{
    vars->astroot = build_ast(vars);
    if (vars->astroot)
    {
        fprintf(stderr, "DEBUG: Built AST successfully\n");
        if (vars->astroot->args && vars->astroot->args[0])
            fprintf(stderr, "DEBUG: Root command: %s\n", 
                vars->astroot->args[0]);
        execute_cmd(vars->astroot, vars->env, vars);
    }
    else
        fprintf(stderr, "DEBUG: Failed to build AST\n");
}

/*
Handles token creation and basic processing.
- Cleans up previous token list if needed.
- Tokenizes and creates lexical tree from input.
- Handles any unclosed quotes by prompting for completion.
Returns:
- Updated command string after processing.
- NULL on memory allocation failure or other error.
Works with process_command() as first processing stage.

Example: For input "echo "hello
- Tokenizes the initial content 
- Detects unclosed quotes and prompts for completion
- Returns completed command string with proper quotes
*/
char	*process_input_tokens(char *command, t_vars *vars)
{
    char	*processed_cmd;

    processed_cmd = command;
    if (vars->head)
    {
        fprintf(stderr, "DEBUG: Cleaning up previous tokens\n");
        cleanup_token_list(vars);
    }
    if (!processed_cmd)
        return (NULL);
        
    fprintf(stderr, "DEBUG: Processing: '%s'\n", processed_cmd);
    tokenize(processed_cmd, vars);
    lexerlist(processed_cmd, vars);
    
    if (vars->quote_depth > 0)
    {
        processed_cmd = handle_quote_completion(processed_cmd, vars);
    }
    
    return (processed_cmd);
}

/*
Handles pipe syntax validation and completion.
- Checks for pipe syntax errors.
- Handles unfinished pipes by prompting for continuation.
Returns:
- Updated command string after pipe processing.
- NULL on memory allocation failure or other error.
- Command unchanged if no pipe issues detected.
Works with process_command() as second processing stage.

Example: For input "ls |"
- Detects unfinished pipe
- Prompts for continuation
- Returns completed command with pipe and continuation
*/
char	*process_pipe_syntax(char *command, char *orig_cmd, t_vars *vars)
{
    int		syntax_chk;
    char	*processed_cmd;
    
    processed_cmd = command;
    syntax_chk = chk_pipe_syntax_err(vars);
    
    if (syntax_chk == 1)
    {
        if (processed_cmd != orig_cmd)
            ft_safefree((void **)&processed_cmd);
        ft_safefree((void **)&orig_cmd);
        return (NULL);
    }
    processed_cmd = handle_pipe_completion(processed_cmd, vars, syntax_chk);
    return (processed_cmd);
}

/*
Process the user command through lexing and execution.
- Handles input tokenization, syntax checking, and execution.
- Breaks processing into smaller logical stages.
- Manages memory throughout command processing lifecycle.
Returns:
- 1 to continue shell loop.
- 0 to exit.
Works with main() in command processing loop.

Example: When user types a complex command
- Processes tokens and handles unclosed quotes
- Validates and completes pipe syntax if needed
- Builds and executes command if valid
- Frees all temporary resources
*/
int	process_command(char *command, t_vars *vars)
{
    char	*processed_cmd;
    
    processed_cmd = process_input_tokens(command, vars);
    if (!processed_cmd)
        return (1);
    processed_cmd = process_pipe_syntax(processed_cmd, command, vars);
    if (!processed_cmd)
        return (1);
    debug_print_token_list(vars);
    build_and_execute(vars);
    if (processed_cmd != command)
        ft_safefree((void **)&processed_cmd);
    ft_safefree((void **)&command);
    return (1);
}

/*
Main entry point for the minishell program.
- Initializes shell environment and variables.
- Enters command processing loop.
- Handles cleanup on exit.
Returns:
0 on normal program exit.
Works as program entry point.
*/
int	main(int ac, char **av, char **envp)
{
    t_vars	vars;
    char	*command;

    (void)ac;
    (void)av;
    init_shell(&vars, envp);
    while (1)
    {
        command = reader(&vars);
        if (!process_command(command, &vars))
            break ;
    }
    if (vars.env)
    {
        ft_free_2d(vars.env, ft_arrlen(vars.env));
        vars.env = NULL;
    }
    cleanup_exit(&vars);
    return (0);
}
