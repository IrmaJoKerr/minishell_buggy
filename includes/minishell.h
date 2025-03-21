/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:16:53 by bleow             #+#    #+#             */
/*   Updated: 2025/03/21 11:34:55 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <string.h>
# include <limits.h>
# include <signal.h>
# include <string.h>
# include <termios.h>
# include <fcntl.h> 
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>

extern volatile sig_atomic_t	g_signal_received;

/*
HISTORY_FILE - Stores the history from previous session
			   and is loaded into memory on startup.
HISTORY_FILE_TMP - Temporary file for copying history
				   to prevent data loss.
HISTORY_FILE_MAX - Maximum number of lines in history file.
HIST_MEM_MAX - Maximum number of lines to load in memory using add_history.
HIST_BUFFER_SZ - Buffer size for reading history file in bytes.
HIST_LINE_SZ - Buffer size for reading each history line in bytes.
*/
# define PROMPT "bleshell$> "
# define HISTORY_FILE "bleshell_history"
# define HISTORY_FILE_TMP "bleshell_history_tmp"
# define HISTORY_FILE_MAX 2000
# define HIST_MEM_MAX 1000
# define HIST_BUFFER_SZ 4096
# define HIST_LINE_SZ 1024

/*
String representations of token types.
These constants match the enum e_tokentype values.
This enables easy conversion between enum and string.
*/
# define TOKEN_TYPE_NULL			 "NULL"
# define TOKEN_TYPE_STRING           "STRING"
# define TOKEN_TYPE_CMD              "CMD"
# define TOKEN_TYPE_ARGS             "ARGS"
# define TOKEN_TYPE_DOUBLE_QUOTE     "\""
# define TOKEN_TYPE_SINGLE_QUOTE     "'"
# define TOKEN_TYPE_HEREDOC          "<<"
# define TOKEN_TYPE_IN_REDIRECT      "<"
# define TOKEN_TYPE_OUT_REDIRECT     ">"
# define TOKEN_TYPE_APPEND_REDIRECT  ">>"
# define TOKEN_TYPE_EXPANSION        "$"
# define TOKEN_TYPE_PIPE             "|"
# define TOKEN_TYPE_EXIT_STATUS      "$?"

/*
This structure is used to store the context of quotes.
Example: "'Hello 'world'!'" has 2 quotes, one single and one double.
*/
typedef struct s_quote_context
{
	char	type;
	int		start_pos;
	int		depth;
}	t_quote_context;

/*
This enum stores the possible token types.
*/
typedef enum e_tokentype
{
	TYPE_NULL = 0,
	TYPE_STRING = 1,
	TYPE_CMD = 2,
	TYPE_ARGS = 3,
	TYPE_DOUBLE_QUOTE = 4,
	TYPE_SINGLE_QUOTE = 5,
	TYPE_HEREDOC = 6,
	TYPE_IN_REDIRECT = 7,
	TYPE_OUT_REDIRECT = 8,
	TYPE_APPEND_REDIRECT = 9,
	TYPE_EXPANSION = 10,
	TYPE_PIPE = 11,
	TYPE_EXIT_STATUS = 12,
}	t_tokentype;

/*
Node structure for linked list and AST.
Next and prev are for building linked list.
Left and right are for building AST.
*/
typedef struct s_node
{
	t_tokentype		type;
	char			**args;
	struct s_node	*next;
	struct s_node	*prev;
	struct s_node	*left;
	struct s_node	*right;
}	t_node;

/*
Structure for storing execution context.
Has variables tracking current execution state.
*/
typedef struct s_exec
{
	char    *cmd_path;     // Path to the command being executed
	pid_t   pid;           // Process ID of the command
	int     status;        // Exit status of the command
	int     result;        // Result of the execution
	t_node  *cmd;          // Command node being executed
	int     append;        // Append mode flag for redirections
} t_exec;

/*
Structure for storing AST (Abstract Syntax Tree) building state.
Has variables tracking:
- Current node processing state
- Command and redirection references
- Syntax validation information
- Pipe structure analysis
- Tree construction progress
This structure centralizes AST building information to simplify
function signatures and improve maintainability. It holds temporary
references needed during the parsing phase but not required after
the AST is fully constructed.
*/
typedef struct s_ast
{
	t_node  *current;        // Current node being processed in the token list
	t_node  *last_cmd;       // Last command node encountered during parsing
	t_node  *last_heredoc;   // Last heredoc node encountered during parsing
	t_node  *cmd_redir;      // Command node being targeted for redirection
	t_node  *pipe_root;      // Root node of pipe structure (ADDED)
	t_node  *root;           // Root node of the AST (ADDED)
	int     cmd_idx;         // Current index in the cmd_nodes array being processed
	int     syntax_error;    // Syntax error code (0: none, 1: error, 2: incomplete)
	int     serial_pipes;    // Counter for detecting consecutive pipes (syntax error)
	int     pipe_at_front;   // Flag indicating pipe at beginning (syntax error)  
	int     pipe_at_end;     // Flag indicating pipe at end (requires more input)
	int     fd_write;        // File descriptor for writing (ADDED for heredoc)
	int     expand_vars;     // Flag for expanding variables (ADDED for heredoc)
} t_ast;

/*
Structure for storing pipeline information.
Has variables tracking:
- Pipe structure
- Execution resources
- File descriptors
- AST structure
- Last command status
*/
typedef struct s_pipe
{
	/* Pipe structure */
	int         pipe_count;     // Number of pipes in the chain
	t_node      **exec_cmds;     // Array of command nodes for execution
	t_node      *cmd_nodes[100]; // Command nodes from parsing phase
	int         cmd_count;      // Count of command nodes 
	int         *pipe_fds;      // Array of pipe file descriptors
	pid_t       *pids;          // Array of process IDs
	int         *status;        // Status for each process
	int         saved_stdin;    // Saved standard input
	int         saved_stdout;   // Saved standard output
	int         heredoc_fd;     // File descriptor for heredoc if present
	t_node      *root_node;     // Root node of the pipe structure
	int         append_mode;    // Append flag for redirections
	t_node      *current_redirect; // Current redirection node
	int         last_cmdcode;    // Status of the last command (for return value)
} t_pipe;

/*
Main structure for storing variables and context.
Makes it easier to access and pass around.
Has variables tracking: 
- Tokenization state
- AST structure
- Shell state
- Current pipeline
*/
typedef struct s_vars
{
	t_node      	*cmd_nodes[100];
	int				cmd_count;
	t_node			*astroot;
	t_node			*head;
	t_node			*current;
	t_tokentype		curr_type;
	t_tokentype		prev_type;
	char			**env;
	t_quote_context	quote_ctx[32];
	int				quote_depth;
	int				pos;
	int				start;
	int				shell_level;
	int				error_code;
	char			*error_msg;
	t_pipe          *pipeline;     // Current pipeline being executed
} t_vars;

/* Builtin commands functions. In srcs/builtins directory. */

/*
builtin_cd.c - Builtin "cd" command. Changes the current working directory.
In builtin_cd.c
*/
int			builtin_cd(char **args, t_vars *vars);
int			handle_cd_special(char **args, t_vars *vars);
int			handle_cd_path(char **args, t_vars *vars);
int			update_env_pwd(t_vars *vars, char *oldpwd);

/*
Builtin "echo" command. Outputs arguments to STDOUT.
In builtin_echo.c
*/
int			builtin_echo(char **args, t_vars *vars);
int			process_echo_args(char **args, int start, int nl_flag);

/*
Builtin "env" command. Outputs the environment variables.
In builtin_env.c
*/
int			builtin_env(t_vars *vars);

/*
Builtin "exit" command. Exits the shell.
In builtin_exit.c
*/
int			builtin_exit(t_vars *vars);

/*
Builtin "export" command utility functions.
In builtin_export_utils.c
*/
char		*valid_export(char *args);
char		**asc_order(char **sort_env, int count);
char		**make_sorted_env(int count, t_vars *vars);
int			process_export_var(char *env_var);
int			process_var_with_val(char *name, char *value);

/*
Builtin "export" command. Sets an environment variable.
In builtin_export.c
*/
int			builtin_export(char **args, t_vars *vars);
int			export_without_args(t_vars *vars);
int			export_with_args(char **args, t_vars *vars);
int			sort_env(int count, t_vars *vars);

/*
Builtin "pwd" command. Outputs the current working directory.
In builtin_pwd.c
*/
int			builtin_pwd(t_vars *vars);

/*
Builtin "unset" command utility functions.
In builtin_unset_utils.c
*/
void		copy_env_front(char **src, char **dst, int pos);
void		copy_env_back(char **src, char **dst, int idx, int offset);

/*
Builtin "unset" command. Unsets an environment variable.
In builtin_unset.c
*/
int			builtin_unset(char **args, t_vars *vars);
int			set_next_pos(int changes, char **env, int pos);
char		**realloc_until_var(int changes, char **env, char *var, int count);
int			get_env_pos(char *var, char **env);
void		modify_env(char ***env, int changes, char *var);

/* Main minishell functions. In srcs directory. */

/*
Argument handling.
In arguments.c
*/
void		create_args_array(t_node *node, char *token);
void		append_arg(t_node *node, char *new_arg);

/*
AST token processing and AST tree building utility functions.
In buildast_utils.c
*/

/*
AST token processing and AST tree building.
In buildast.c
*/
void		get_cmd_nodes(t_vars *vars);
t_node		*find_next_cmd(t_node *start);
void		setup_first_pipe(t_node *pipe_node, t_node *prev_cmd, t_node *next_cmd);
void		setup_next_pipes(t_node *pipe_node, t_node *last_pipe, t_node *last_cmd, t_node *next_cmd);
t_node		*proc_pipes_pt1(t_vars *vars, t_node **last_pipe, t_node **last_cmd);
void		proc_pipes_pt2(t_vars *vars, t_node *pipe_root, t_node **last_pipe, t_node **last_cmd);
void		setup_redir_ast(t_node *redir, t_node *cmd, t_node *target);
void		upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir);
int			is_redir_token(t_tokentype type);
int			is_valid_redir_node(t_node *current);
t_node		*get_redir_target(t_node *current, t_node *last_cmd);
t_node		*proc_redir_pt1(t_vars *vars, t_node *pipe_root);
void		proc_redir_pt2(t_vars *vars, t_node *pipe_root);
t_node		*proc_token_list(t_vars *vars);
void		convert_str_to_cmds(t_vars *vars);
void		setup_pipe_links(t_node *pipe_node, t_node *left_cmd,
				t_node *right_cmd);
void		convert_strs_to_cmds(t_vars *vars);
void		del_list_node(t_node *node);
int			is_special_token(t_node *token);
void		handle_quoted_arg(t_node *cmd_node, t_node *quote_token);
int			is_operator_token(t_node *token);
void		link_strargs_to_cmds(t_vars *vars);
void		debug_print_pipe_info(t_node *pipe_node, char *position_msg);
void		link_addon_pipe(t_node *last_pipe, t_node *new_pipe, t_node *right_cmd);
void		build_pipe_ast(t_vars *vars);
void		process_token_list(t_vars *vars);
t_node		*set_ast_root(t_node *pipe_node, t_vars *vars);
int			chk_start_pipe(t_vars *vars);
int			detect_multi_pipes(t_vars *vars, int pipes_count);
int			detect_adj_pipes(t_vars *vars, t_node *current);
int			chk_next_pipes(t_vars *vars);
int			chk_end_pipe(t_vars *vars);
int			chk_pipe_syntax_err(t_vars *vars);
char		*merge_input(char *input, char *line);
char		*handle_trailing_pipe_pt1(char *input, t_vars *vars);
char		*handle_trailing_pipe_pt2(char *new_input, t_vars *vars);
char		*handle_incomplete_pipe(char *input, t_vars *vars);
t_node		*build_ast(t_vars *vars);
void		debug_print_token_attrib(t_node *current, int i);
void		debug_print_token_list(t_vars *vars);
int			check_initial_pipe(t_vars *vars, t_ast *ast);
int			check_bad_pipe_series(t_vars *vars);

/*
Builtin control handling.
In builtin.c
*/
int			is_builtin(char *cmd);
int			execute_builtin(char *cmd, char **args, t_vars *vars);

/*
Group A of cleanup functions.
In cleanup_a.c
*/
void		cleanup_env_error(char **env, int n);
void		cleanup_ast_struct(t_ast *ast);
void		cleanup_pipeline(t_pipe *pipeline);
void		cleanup_vars(t_vars *vars);
void		cleanup_exit(t_vars *vars);

/*
Group B of cleanup functions.
In cleanup_b.c
*/
void		cleanup_ast(t_node *node);
void		free_token_node(t_node *node);
void		cleanup_token_list(t_vars *vars);
void		cleanup_exec_context(t_exec *exec);
void		cleanup_fds(int fd_in, int fd_out);

/*
Error handling.
In errormsg.c
*/
void		file_access_error(char *filename);
void		use_errno_error(char *filename, int *error_code);
int			redirect_error(char *filename, t_vars *vars, int use_errno);
int			print_error(const char *msg, t_vars *vars, int error_code);
void		crit_error(t_vars *vars);

/*
Execution functions.
In execute.c
*/
int			handle_cmd_status(int status, t_vars *vars);
int			setup_out_redir(t_node *node, int *fd, int append);
int			setup_in_redir(t_node *node, int *fd);
int			setup_redirection(t_node *node, t_vars *vars, int *fd);
int			exec_redirect_cmd(t_node *node, char **envp, t_vars *vars);
int			exec_child_cmd(t_node *node, char **envp, t_vars *vars, char *cmd_path);
void		print_cmd_args(t_node *node);
int			exec_std_cmd(t_node *node, char **envp, t_vars *vars);
int			execute_cmd(t_node *node, char **envp, t_vars *vars);

/*
Expansion handling.
In expansion.c
*/
char		*chk_exitstatus(t_vars *vars);
char		*handle_special_var(const char *var_name, t_vars *vars);
char		*get_env_val(const char *var_name, char **env);
char		*get_var_name(char *input, int *pos);
char		*append_char(char *str, char c);
char		*handle_expansion(char *input, int *pos, t_vars *vars);
int			expand_one_arg(char **arg, t_vars *vars);
void		expand_cmd_args(t_node *node, t_vars *vars);

/*
Heredoc checking and utility functions.
In heredoc_checks_and_utils.c
*/

/*
Heredoc execution functions.
In heredoc_execute.c
*/

/*
Heredoc expansion handling.
In heredoc_expand.c
*/

/*
Heredoc pasted text handling.
In heredoc_pasted.c
*/

/*
Heredoc main handling.
In heredoc.c
*/
char		*merge_and_free(char *str, char *chunk);
char		*expand_heredoc_var(char *line, int *pos, t_vars *vars);
char		*read_heredoc_str(char *line, int *pos);
char		*expand_one_line(char *line, int *pos, t_vars *vars, char *result);
char		*expand_heredoc_line(char *line, t_vars *vars);
int			chk_expand_heredoc(char *delimiter);
int			write_to_heredoc(int fd, char *line, t_vars *vars, int expand_vars);
int			read_heredoc(int *fd, char *delimiter, t_vars *vars, int expand_vars);
int			handle_heredoc_err(t_node *node, t_vars *vars);
int			cleanup_heredoc_fail(int *fd, t_vars *vars);
int			handle_heredoc(t_node *node, t_vars *vars);
int			proc_heredoc(t_node *node, t_vars *vars);

/*
History loading functions.
In history_load.c
*/
void		skip_history_lines(int fd, int skip_count);
void		read_history_lines(int fd);
void		load_history(void);

/* 
History saving utility functions.
In history_save_utils.c
*/

/*
History saving functions.
In history_save.c
*/
int			copy_file_content(int fd_src, int fd_dst);
int			copy_file(const char *src, const char *dst);
int			copy_to_temp(int fd_read);
void		skip_lines(int fd, int count);
void		trim_history(int excess_lines);
void		save_history(void);
int			save_history_entries(int fd, HIST_ENTRY **hist_list, int start, int total);

/*
History main functions.
In history.c
*/
int			init_history_fd(int mode);
int			append_history(int fd, const char *line);
int			get_history_count(void);

/*
Node initialization functions.
In initnode.c
*/
int			make_nodeframe(t_node *node, t_tokentype type, char *token);
t_node		*initnode(t_tokentype type, char *token);

/*
Shell and structure initialization functions.
In initshell.c
*/
t_ast		*init_ast_struct(void);
t_exec		*init_exec_context(t_vars *vars);
t_ast		*init_verify(char *input, char **cmd_ptr);
void		init_lexer(t_vars *vars);

/*
Input completion functions.
In input_completion.c
*/
int			is_input_complete(t_vars *vars);
int			check_unfinished_pipe(t_vars *vars, t_ast *ast);
int			handle_unfinished_pipes(char **processed_cmd, t_vars *vars,
							t_ast *ast);
char		*get_quote_input(t_vars *vars);
int			chk_quotes_closed(char **processed_cmd, t_vars *vars);
int			quotes_are_closed(const char *str);
int			handle_unclosed_quotes(char **processed_cmd, t_vars *vars);
char		*append_input(const char *first, const char *second);


/*
Input verification functions.
In input_verify.c
*/
int			tokenize_to_test(char *input, t_vars *vars);
int			process_input_addons(char **processed_cmd, t_vars *vars, t_ast *ast);
int			chk_pipe_before_cmd(t_vars *vars, t_ast *ast);
int			chk_serial_pipes(t_vars *vars, t_ast *ast);
int			chk_syntax_errors(t_vars *vars);
int			chk_input_valid(t_vars *vars, char **input);
char		*verify_input(char *input, t_vars *vars);
int			count_tokens(t_node *head);
int			prepare_input(char *input, t_vars *vars, char **processed_cmd);
char		*join_with_newline(char *first, char *second);
char		*append_new_input(char *first, char *second);

/*
Lexer utility functions.
In lexer_utils.c
*/

/*
Lexer functions.
In lexer.c
*/
void		skip_whitespace(char *str, t_vars *vars);
char		*read_added_input(char *prompt);
void		handle_text_chunk(char *str, t_vars *vars);
void		process_text(char *str, t_vars *vars, int *first_token, t_tokentype override_type);	
void		handle_quote_content(char *str, t_vars *vars, int *first_token);
char		*lexing_unclosed_quo(char *input, t_vars *vars);
void		handle_expansion_token(char *str, t_vars *vars, int *first_token);
void		handle_token_boundary(char *str, t_vars *vars, int *first_token);
void		create_operator_token(t_vars *vars, t_tokentype type, char *symbol);
void		handle_operator_token(char *str, t_vars *vars, int *first_token);
void		handle_token(char *str, t_vars *vars);
void		lexerlist(char *str, t_vars *vars);

/*
Minishell program entry point functions.
In minishell.c
*/
char		*reader(t_vars *vars);
void		setup_env(t_vars *vars, char **envp);
void		init_shell(t_vars *vars, char **envp);
char		*handle_quote_completion(char *cmd, t_vars *vars);
char		*handle_pipe_valid(char *cmd, t_vars *vars, int syntax_chk);
void		build_and_execute(t_vars *vars);
char		*process_input_tokens(char *command, t_vars *vars);
char		*process_pipe_syntax(char *command, char *orig_cmd, t_vars *vars);
int			process_command(char *command, t_vars *vars);
int			main(int ac, char **av, char **envp);

/*
Node handling.
In nodes.c
*/
int			makenode(t_vars *vars, char *data);
void		add_child(t_node *parent, t_node *child);
void		handle_pipe_node(t_node **root, t_node *pipe_node);
void		redirection_node(t_node *root, t_node *redir_node);

/*
Operator handling.
In operators.c
*/
int			operators(char *input, int i, int token_start, t_vars *vars);
int			handle_string(char *input, int i, int token_start, t_vars *vars);
t_tokentype	get_operator_type(char op);
int			handle_single_operator(char *input, int i, t_vars *vars);
int			handle_double_operator(char *input, int i, t_vars *vars);

/*
Path finding functions.
In paths.c
*/
char		**get_path_env(char **envp);
char		*try_path(char *path, char *cmd);
char		*search_in_env(char *cmd, char **envp);
char		*get_cmd_path(char *cmd, char **envp);
char		**dup_env(char **envp);

/*
Pipeline handling utility functions.
In pipeline_utils.c
*/

/*
Pipeline main handling functions.
In pipeline.c
*/

/*
Pipes execution functions.
In pipes_execution.c
*/

/*
Pipes syntax checking functions.
In pipes_syntax.c
*/

/*
Pipes main functions.
In pipes.c
*/
void		init_pipe(t_node *cmd, int *pipe_fd);
int			validate_pipe_node(t_node *pipe_node);
int			setup_pipe(int *pipefd);
void		exec_left_cmd(t_node *pipe_node, int *pipefd, t_vars *vars);
void		exec_right_cmd(t_node *pipe_node, int *pipefd, t_vars *vars);
pid_t		make_child_proc(t_node *pipe_node, int *pipefd, t_vars *vars,
					int is_left);
void		reset_done_pipes(t_ast *ast, char **pipe_cmd, char **result,
						int free_flags);
int			prep_pipe_complete(char *cmd, char **result, char **pipe_cmd,
						t_ast **ast);
char		*handle_pipe_completion(char *cmd, t_vars *vars, int syntax_chk);
int			setup_pipeline_procs(t_node *pipe_node, t_vars *vars,
				pid_t *left_pid, pid_t *right_pid);
int			execute_pipeline(t_node *pipe_node, t_vars *vars);

/*
Quote handling.
In quotes.c
*/
void		handle_quotes(char *input, int *pos, t_vars *vars);
char		*fix_open_quotes(char *input, t_vars *vars);
char		*read_quoted_content(char *input, int *pos, char quote);
void		strip_quotes(char **str_ptr, char quote_char);
void		process_quotes_in_arg(char **arg);
int			scan_for_endquote(char *str, int *pos, char quote_char);
void		valid_quote_token(char *str, t_vars *vars, int *pos, int start);

/*
Rediretion AST node handling.
In redirect_ast.c
*/

/*
Redirection processing functions.
In redirect_process.c
*/

/*
Redirection utility functions.
In redirect_utils.c
*/

/*
Redirection handling.
In redirect.c
*/
int			chk_permissions(char *filename, int mode, t_vars *vars);
int			set_output_flags(int append);
int			set_redirect_flags(int mode);
int			is_redirection(t_tokentype type);
void		reset_redirect_fds(t_vars *vars);
int			input_redirect(t_node *node, int *fd_in, t_vars *vars);
int			output_redirect(t_node *node, int *fd_out, int append, t_vars *vars);
int			open_redirect_file(t_node *node, int *fd, int mode, t_vars *vars);
int			handle_redirect(t_node *node, int *fd, int mode, t_vars *vars);

/*
Shell level handling.
In shell_level.c
*/
int			get_shell_level(t_vars *vars);
int			update_shlvl_env(char **env, int position, int new_level);
int			incr_shell_level(t_vars *vars);

/*
Signal handling.
In signals.c
*/
void		load_signals(void);
void		sigint_handler(int sig);
void		sigquit_handler(int sig);

/*
Token classification handling.
In tokenclass.c
*/
char		*handle_exit_status(t_vars *vars);
t_tokentype	redirection_type(char *str, int mode, t_tokentype type, int pos);
t_tokentype	classify(char *str, int pos);

/*
Tokenizing utility functions.
In tokenize_utils.c
*/

/*
Tokenizing functions.
In tokenize.c
*/
void		maketoken(char *token, t_vars *vars);
t_node		*find_last_command(t_node *head);
int			handle_expand(t_vars *vars);
int			process_special_char(char *input, int *i, t_vars *vars);
int			process_expand_char(char *input, int *i, t_vars *vars);
int			process_quote_char(char *input, int *i, t_vars *vars);
int			process_operator_char(char *input, int *i, t_vars *vars);
void		process_char(char *input, int *i, t_vars *vars);
int			scan_quote_position(char *str, int *pos, char quote_char);
void		create_quote_token(char *str, t_vars *vars, int *pos, int start);
void		handle_quote_token(char *str, t_vars *vars, int *pos);
void		handle_redirection(char *input, int *i, t_vars *vars);
void		tokenize(char *input, t_vars *vars);
void		process_other_token(char *input, t_vars *vars);
t_node		*make_cmdnode(char *token);
t_node		*new_cmd_node(char *token);
t_node		*new_other_node(char *token, t_tokentype type);
void		build_token_linklist(t_vars *vars, t_node *node);
void		process_cmd_token(char *input, t_vars *vars);
int			is_flag_arg(char **args, int i);
void		join_flag_args(char **args, int i);
t_node		*build_cmdarg_node(char **args);
void		process_args_tokens(char **args);

/*
Type conversion functions.
In typeconvert.c
*/
const char	*get_token_str_basic(t_tokentype type);
const char	*get_token_str(t_tokentype type);
t_tokentype	get_token_type_basic(const char *str);
t_tokentype	get_token_type(const char *str);

#endif