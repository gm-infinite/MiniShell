/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 19:11:29 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/24 21:54:32 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <readline/readline.h>
# include <readline/history.h>
# define _POSIX_C_SOURCE 200809L
# include <signal.h>
# include <sys/wait.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <termios.h>
# include <sys/ioctl.h>
# include <limits.h>
# include "../e-libft/libft.h"
# include "../parser/t_split_utils/t_split.h"

# ifndef PATH_MAX
#  define PATH_MAX 4096
# endif

extern volatile sig_atomic_t	g_signal;

/*
** ============================================================================
**                        CORE DATA STRUCTURES
** ============================================================================
*/
typedef struct s_shell
{
	int		past_exit_status;
	char	*current_input;
	t_split	split_input;
	char	**envp;
	int		should_exit;
	int		exit_code;
	int		in_subshell;
	char	*prompt;
	char	*terminal_prompt;
}	t_shell;

typedef struct s_heredoc_sub
{
	int		temp_fd;
	int		should_expand;
	char	**args;
}	t_heredoc_sub;

typedef struct s_pipe_context
{
	int		**all_pipes;
	int		pipe_count;
	int		input_fd;
	int		output_fd;
}			t_pipe_context;

typedef struct s_pipe_ctx
{
	t_split	*commands;
	int		**pipes;
	pid_t	*pids;
	int		cmd_count;
	t_shell	*shell;
}	t_pipe_ctx;

typedef struct s_redir_exec_ctx
{
	char	**args;
	int		input_fd;
	int		output_fd;
	int		stderr_fd;
	t_shell	*shell;
}	t_redir_exec_ctx;

typedef struct s_pipe_setup_ctx
{
	int		cmd_index;
	int		cmd_count;
	int		**pipes;
	int		*input_fd;
	int		*output_fd;
}	t_pipe_setup_ctx;

typedef struct s_process_pipe_ctx
{
	t_split	*commands;
	int		**pipes;
	pid_t	*pids;
	int		cmd_count;
	t_shell	*shell;
}	t_process_pipe_ctx;

typedef struct s_redir_fds
{
	int	*input_fd;
	int	*output_fd;
	int	*stderr_fd;
}	t_redir_fds;

typedef struct s_heredoc_params
{
	char	*delimiter;
	char	**args;
	char	**clean_args;
}	t_heredoc_params;

typedef struct s_child_ctx
{
	int		**pipes;
	int		cmd_index;
	int		cmd_count;
}			t_child_ctx;

typedef struct s_child_params
{
	t_split					cmd;
	t_child_ctx	*ctx;
	t_shell					*shell;
	t_split					*commands;
	pid_t					*pids;
}	t_child_params;

typedef struct s_child_redir_params
{
	t_split					cmd;
	t_child_ctx	*ctx;
	t_shell					*shell;
	t_split					*commands;
	pid_t					*pids;
}	t_child_redir_params;

typedef struct s_paren_info
{
	int		first_idx;
	int		last_idx;
	int		open_count;
	int		close_count;
}	t_paren_info;

typedef struct s_expand
{
	char	*result;
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	char	*expanded;
	int		indx;
	int		var_len;
}	t_expand;

typedef struct s_redirect_info
{
	int		redirect_type;
	char	*processed_filename;
}	t_redirect_info;

typedef struct s_pipe_cleaner
{
	t_split	*commands;
	int		**pipes;
	pid_t	*pids;
	int		cmd_count;
}	t_pipe_cleaner;

/*
** ────────────────────────────────────────────────────────────────────────────
**                           CORE SHELL FUNCTIONS
** ────────────────────────────────────────────────────────────────────────────
** Primary shell control
*/
void					shell_init(t_shell *shell);
void					safe_exit(t_shell *shell);
int						is_empty(char *str);

/*				
** ─────				───────────────────────────────────────────────────────────────────────
**      				                     SIGNAL HANDLING SUBSYSTEM
** ─────				───────────────────────────────────────────────────────────────────────
** Compr				ehensive signal management
*/				
void					setup_signals(void);
void					signal_handler_child(int sig);
void					setup_child_signals(void);

/*				
** ─────				───────────────────────────────────────────────────────────────────────
**      				                  PARSING AND TOKENIZATION SUBSYSTEM
** ─────				───────────────────────────────────────────────────────────────────────
** Advan				ced command line parsing with operator precedence and grouping support
*/				
void					parser_and_or(t_shell *shell, t_split split);
int						is_andor(char *str);
t_split					paren_in_split(t_split cmd, t_shell *shell);
void					process_segment(t_shell *shell, t_split split, int *vars, char *c_i);
int						is_singlepar(char *start, char *end);
void					remove_outer_par(char *start, char *end);
void					clean_empties(t_split *split);
int						validate_exec(char **args, char *executable);
void					handle_error_and_exit(char **args, char *executable,
							t_shell *shell);
void					error_pipe_exec(char **args, char *executable,
							t_shell *shell, t_pipe_cleaner *cleanup);
int						countchr_str(char *str, char c);
int						countchr_not_quote(char *str, char c);
void					sep_opt_arg(t_shell *shell);
int						count_str_split(t_split split, const char *str, int flag);
char					*get_cut_indexs(t_split split);
void					cut_out_par(t_split *split);
int						check_op_errs(t_split split);
int						check_par_errs(t_split split);

/*
** ────────────────────────────────────────────────────────────────────────────
**                      ENVIRONMENT VARIABLE SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Dynamic environment management with expansion and modification support
*/
char					*expandvar(char *str, t_shell *shell);
char					*tilde(char *str, t_shell *shell);
char					*get_env_value(char *var_name, t_shell *shell);
void					init_env(t_shell *shell, char **envp);
void					free_env(t_shell *shell);
int						set_env_var(char *var_name, char *value, t_shell *shell);
int						replacevar(t_expand *holder, int flags);
char					*expandvar_q(char *str, t_shell *shell);
int						expand_dollar(t_expand *holder, t_shell *shell);
int						expand_double_q(t_expand *holder);
int						expand_single_q(t_expand *holder);
void					init_expand_holder(t_expand *holder, char *tilde_expanded);
void					expander_loop(t_expand *holder, t_shell *shell);
int						handle_special_chars(t_expand *holder);

/*
** ────────────────────────────────────────────────────────────────────────────
**                          BUILT-IN COMMANDS SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Implementation of shell built-in commands with proper argument handling
*/
int						is_builtin(char *cmd);
int						execute_builtin(char **args, t_shell *shell);
int						is_valid_var_name(const char *name);
int						builtin_echo(char **args);
int						builtin_cd(char **args, t_shell *shell);
int						home_path(char **path, char **home, t_shell *shell);
int						oldpwd_path(char **path, t_shell *shell);
int						builtin_pwd(void);
int						builtin_export(char **args, t_shell *shell);
int						builtin_unset(char **args, t_shell *shell);
int						builtin_env(t_shell *shell);
int						builtin_exit(char **args, t_shell *shell);
int						append_env_var(char *var_name, char *value, t_shell *shell);
void					export_error(char *arg);
void					option_error(char *arg, t_shell *shell);
int						process_assign_export(char *arg, char *eq_pos, t_shell *shell);
int						process_append_export(char *arg, char *eq_pos, t_shell *shell);
int						process_single_export(char *arg, t_shell *shell);
int						new_env_var(char *var_name, char *value, t_shell *shell);
int						new_empty_envvar(char *var_name, t_shell *shell);
int						copy_env_without_var(t_shell *shell, int remove_index, int count);
int						find_env_var_index(t_shell *shell, char *var_name, int len);
int						count_env_vars(t_shell *shell);

/*
** ────────────────────────────────────────────────────────────────────────────
**                        COMMAND EXECUTION SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Comprehensive command execution with pipeline and redirection support
*/
int						execute_command(t_split split, t_shell *shell);
char					**arg_expander_loop(char **args, t_shell *shell);
char					*reconstructed_args(char **args);
char					*wildcard_expand(char *reconstructed);
int						execute_expanded_args(char *reconstructed, t_shell *shell);
int						execute_pipeline(t_split split, t_shell *shell);
int						execute_pipeline_with_redirections(t_split split, t_shell *shell);
void					redirect_fds(int input_fd, int output_fd, int stderr_fd);
void					exec_err(char *cmd, char *message);
void					execute_child_command(char **args, t_shell *shell);
int						execute_single_command(char **args, t_shell *shell);
int						count_pipes(t_split split);
t_split					*split_by_pipes(t_split split, int *cmd_count);
int						execute_pipe_command(t_split cmd, t_pipe_context *ctx, t_shell *shell);
void					expand_command_args(char **args, t_shell *shell);
int						handle_builtin_command(char **args, t_pipe_context *ctx,
							t_shell *shell);
int						handle_external_command(char **args, t_pipe_context *ctx,
							t_shell *shell);
void					execute_pipe_child(t_child_params *params);
char					**execute_expanded_args_split(char *reconstructed, char **args,
							t_shell *shell);
void					execute_pipe_external_command(char **args, t_shell *shell,
							t_pipe_cleaner *cleanup);
void					setup_child(t_process_pipe_ctx *proc_ctx, int i);
void					setup_pipe_signals(void);
void					free_child_memory(char **args, t_shell *shell);
void					free_heredoc_child_memory(t_shell *shell, t_heredoc_params *params);
void					free_child_pipeline_memory(char **args, t_shell *shell,
							t_pipe_cleaner *cleanup);
void					execute_pipe_child_with_redirections(t_child_redir_params *params);
int						has_parentheses_in_split(t_split split);
int						check_pipe_error(t_split split, int i, int has_parentheses);
int						check_redirection_error(t_split split, int i);
char					*remove_quotes_for_redirection(char *str);
int						create_pipes_array(int ***pipes, int cmd_count);
void					cleanup_pipes(int **pipes, int cmd_count);
void					close_all_pipes(int **pipes, int cmd_count);
void					setup_pipe_fds(t_pipe_setup_ctx *ctx);
int						create_pipes(int ***pipes, int cmd_count);
void					pipe_clean(int **pipes, int cmd_count);
int						wait_for_children(pid_t *pids, int cmd_count);
int						heredoc_redir(char **args, int j,
							t_pipe_ctx *pipeline_ctx, int i);
void					setup_child_redir(t_pipe_context *ctx);
int						setup_pipe(t_split **commands, int ***pipes, pid_t **pids,
							int cmd_count);
int						exec_pipe_child(t_pipe_ctx *pipeline_ctx);
void					clean_pipe(t_split *commands, int **pipes, pid_t *pids,
							int cmd_count);
char					*heredoc_filename(int cmd_index);
int						heredoc_rline(char **line);
void					err_heredoc(char *processed_delimiter);
int						heredoc_content(int temp_fd,
							char *processed_delimiter,
							t_shell *shell, int should_expand);
int						heredoc_pipe(t_pipe_ctx *pipeline_ctx);
int						fork_pipe_child(t_pipe_ctx *pipeline_ctx, int i);

/*
** ────────────────────────────────────────────────────────────────────────────
**                          I/O REDIRECTION SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Comprehensive input/output redirection with file and pipe support
*/
int						is_redirection(char *token);
int						has_redirs(t_split split);
int						validate_redirection_syntax(t_split split);
char					*remove_quotes_for_redirection(char *str);
int						handle_here_doc(int *pipe_fd,
							t_shell *shell, int should_expand,
							t_heredoc_params *params);
char					*read_heredoc(void);
int						is_delimiter_match(char *line, char *delimiter);
char					**parse_redirs(t_split split,
							t_redir_fds *fds, t_shell *shell);
int						execute_with_redirections(t_split split,
							t_shell *shell);
int						process_single_redir(int i, t_redir_fds *fds,
							t_shell *shell, t_heredoc_params *params);
int						handle_here_document(char *processed_filename,
							t_redir_fds *fds, t_shell *shell,
							t_heredoc_params *params);
int						handle_output_redirect(char *processed_filename,
							t_redir_fds *fds, int redirect_type);
char					*process_heredoc_delimiter(char *filename);
int						delimiter_was_quoted(char *filename);
char					*process_filename(char *filename, t_shell *shell);
char					*process_q_expand(char *str,
							t_shell *shell);
int						clean_heredoc(char *processed_filename,
							t_redir_fds *fds);
int						handle_input_redirection(char *processed_filename,
							t_redir_fds *fds);
char					*join_expanded_segment(char *result, char *expanded);
char					*handle_segment_processing(char *result,
							char *str, int *vars, t_shell *shell);
char					*handle_quote_char(char *result, char quote_char);
char					*handle_double_quote_case(char *result, int *i,
							int *in_double_quotes, int in_single_quotes);
char					**build_clean_args(char **args, int clean_count);
int						count_clean_args(char **args);

/*
** ────────────────────────────────────────────────────────────────────────────
**                            QUOTE PROCESSING SYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Advanced quote handling with variable expansion and escape processing
*/
int						check_quotes(char *str);
char					*process_quotes(char *str, t_shell *shell);
void					process_args_quotes(char **args, t_shell *shell);
char					*expandvar_quoted(char *str, t_shell *shell);
char					*remove_quotes_from_string(char *str);
int						handle_escaped_quote(char *str,
							char *result, int i, int j);
void					update_quote_states(char c, int *states);
int						should_copy_character(char c, int *states);
int						process_quote_character(char *str, char *result,
							int *indices, int *states);

/*
** ────────────────────────────────────────────────────────────────────────────
**                              UTILITY FUNCTIONS
** ────────────────────────────────────────────────────────────────────────────
** General-purpose utility functions for common operations
*/
char					**split_to_args(t_split split);
char					**allocate_args_array(int size);
int						copy_split_strings(char **args, t_split split);
char					*revert_split_str(t_split split);
void					free_args(char **args);
int						count_non_empty_args(char **args);
int						copy_non_empty_args(char **args, char **filtered);
void					process_and_check_args(char **args, t_shell *shell);
void					handle_empty_pipe_args(char **args);
char					*find_executable(char *cmd, t_shell *shell);
char					*handle_absolute_path_result(char *result, char *cmd);
char					*handle_no_path_env(char *cmd);
char					*wildcard_input_modify(char *current_input);
int						paranthesis_parity_check(t_split split);
int						check_single_par(t_split split);
int						validate_and_process_args(char **args, t_shell *shell);
int						validate_executable(char *cmd, char *executable);
int						handle_executable_not_found(char **args);
int						has_not_directory_error(char *path);
void					write_not_directory_error(char *filename);
void					handle_error_and_exit(char **args,
							char *executable, t_shell *shell);
void					error_pipe_exec(char **args,
							char *executable, t_shell *shell,
							t_pipe_cleaner *cleanup);
int						execute_builtin_with_redirect(t_redir_exec_ctx *ctx);
int						execute_external_with_redirect(t_redir_exec_ctx *ctx);
int						handle_empty_args(char **args, int input_fd,
							int output_fd, int stderr_fd);
void					process_variable_expansion(char **args, t_shell *shell);
int						check_empty_command_after_expansion(char **args,
							int input_fd, int output_fd, int stderr_fd);
void					execute_heredoc_child(char *delimiter, int pipe_fd,
							t_shell *shell, int should_expand);
int						handle_heredoc_parent(pid_t pid, int *pipe_fd);
void					write_line_to_pipe(int pipe_fd, char *line);
void					setup_pipe_redirection(int cmd_index,
							int cmd_count, int **pipes);
t_pipe_setup_ctx	get_pipe_ctx(int cmd_index, int cmd_count,
							int **pipes, int *fd_values);
t_pipe_cleaner		get_cleanup(t_split *commands,
							int **pipes, pid_t *pids, int cmd_count);

#endif