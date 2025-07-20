/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/20 22:15:01 by emgenc           ###   ########.fr       */
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
}			t_shell;

typedef struct s_pipe_context
{
	int		**all_pipes;
	int		pipe_count;
	int		input_fd;
	int		output_fd;
}			t_pipe_context;

typedef struct s_pipeline_context
{
	t_split	*commands;
	int		**pipes;
	pid_t	*pids;
	int		cmd_count;
	t_shell	*shell;
}	t_pipeline_context;

typedef struct s_redir_exec_context
{
	char	**args;
	int		input_fd;
	int		output_fd;
	int		stderr_fd;
	t_shell	*shell;
}	t_redir_exec_context;

typedef struct s_pipe_setup_context
{
	int		cmd_index;
	int		cmd_count;
	int		**pipes;
	int		*input_fd;
	int		*output_fd;
}	t_pipe_setup_context;

typedef struct s_process_pipeline_context
{
	t_split	*commands;
	int		**pipes;
	pid_t	*pids;
	int		cmd_count;
	t_shell	*shell;
}	t_process_pipeline_context;

typedef struct s_redir_fds
{
	int	*input_fd;
	int	*output_fd;
	int	*stderr_fd;
}	t_redir_fds;

typedef struct s_pipe_child_context
{
	int		**pipes;
	int		cmd_index;
	int		cmd_count;
}			t_pipe_child_context;

typedef struct s_paren_info
{
	int		first_idx;
	int		last_idx;
	int		open_count;
	int		close_count;
}			t_paren_info;

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

/*
** ────────────────────────────────────────────────────────────────────────────
**                           CORE SHELL FUNCTIONS
** ────────────────────────────────────────────────────────────────────────────
** Primary shell control and utility functions for basic operation
*/
void	shell_init(t_shell *shell);
void	safe_exit(t_shell *shell);
int		is_empty(char *str);

/*
** ────────────────────────────────────────────────────────────────────────────
**                           SIGNAL HANDLING SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Comprehensive signal management for interactive and batch operation modes
*/
void	setup_signals(void);
void	signal_handler(int sig);
void	signal_handler_child(int sig);
void	setup_child_signals(void);

/*
** ────────────────────────────────────────────────────────────────────────────
**                        PARSING AND TOKENIZATION SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Advanced command line parsing with operator precedence and grouping support
*/
void	parser_and_or(t_shell *shell, t_split split);
int		countchr_str(char *str, char c);
int		countchr_quote(char *str, char c);
int		countchr_not_quote(char *str, char c);
int		check_single_par(t_split split);
int		check_symbol(t_split split, char *find, int flag);
void	init_parse_vars(int *vars);
int		handle_syntax_validation(t_shell *shell, t_split split);
int		handle_parentheses_processing(t_shell *shell, t_split *split);
void	sep_opt_arg(t_shell *shell);
int		count_str_split(t_split split, const char *str, int flag);
char	*get_cut_indexs(t_split split);
int		paranthesis_parity_check(t_split split);
void	cut_out_par(t_split *split);
int		check_operator_syntax_errors(t_split split);
int		check_parentheses_syntax_errors(t_split split);

/*
** ────────────────────────────────────────────────────────────────────────────
**                      ENVIRONMENT VARIABLE SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Dynamic environment management with expansion and modification support
*/
char	*expand_variables(char *str, t_shell *shell);
char	*expand_tilde(char *str, t_shell *shell);
char	*get_env_value(char *var_name, t_shell *shell);
void	init_environment(t_shell *shell, char **envp);
void	free_environment(t_shell *shell);
int		set_env_var(char *var_name, char *value, t_shell *shell);
int		unset_env_var(char *var_name, t_shell *shell);
int		replace_var_with_value(t_expand *holder, t_shell *shell, int flags);

/*
** ────────────────────────────────────────────────────────────────────────────
**                          BUILT-IN COMMANDS SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Implementation of shell built-in commands with proper argument handling
*/
int		is_builtin(char *cmd);
int		execute_builtin(char **args, t_shell *shell);
int		is_valid_var_name(const char *name);
int		builtin_echo(char **args);
int		builtin_cd(char **args, t_shell *shell);
int		handle_home_path(char **path, char **home, t_shell *shell);
int		handle_oldpwd_path(char **path, t_shell *shell);
int		builtin_pwd(void);
int		builtin_export(char **args, t_shell *shell);
int		builtin_unset(char **args, t_shell *shell);
int		builtin_env(t_shell *shell);
int		builtin_exit(char **args, t_shell *shell);
int		set_env_var(char *var_name, char *value, t_shell *shell);
int		append_env_var(char *var_name, char *value, t_shell *shell);
int		print_sorted_env(t_shell *shell);
int		process_export_args(char **args, t_shell *shell);
void	print_export_error(char *arg);
void	print_option_error(char *arg, t_shell *shell);
int		process_assign_export(char *arg, char *eq_pos, t_shell *shell);
int		process_append_export(char *arg, char *eq_pos, t_shell *shell);
int		process_single_export(char *arg, t_shell *shell);
int		create_new_env_var(char *var_name, char *value, t_shell *shell);
int		append_existing_var(int index, char *var_name, char *value,
			t_shell *shell);
int		unset_env_var(char *var_name, t_shell *shell);
int		unset_check_and_remove(t_shell *shell, char *var_name, int len,
			int index);
int		copy_env_without_var(t_shell *shell, int remove_index, int count);
int		find_env_var_index(t_shell *shell, char *var_name, int len);
int		count_env_vars(t_shell *shell);

/*
** ────────────────────────────────────────────────────────────────────────────
**                        COMMAND EXECUTION SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Comprehensive command execution with pipeline and redirection support
*/
int		execute_command(t_split split, t_shell *shell);
int		execute_pipeline(t_split split, t_shell *shell);
int		execute_pipeline_with_redirections(t_split split, t_shell *shell);
int		execute_single_command(char **args, t_shell *shell);
int		count_pipes(t_split split);
t_split	*split_by_pipes(t_split split, int *cmd_count);
t_split	process_parentheses_in_split(t_split cmd, t_shell *shell);
t_split	handle_parentheses_removal(t_split cmd, t_paren_info info,
			t_shell *shell);
t_split	remove_opening_paren(t_split cmd, int first_idx);
t_split	remove_closing_paren(t_split cmd, int last_idx);
int		execute_pipe_command(t_split cmd, t_pipe_context *ctx, t_shell *shell);
void	execute_pipe_child(t_split cmd, t_pipe_child_context *ctx,
			t_shell *shell);
void	setup_and_execute_child(t_process_pipeline_context *proc_ctx, int i);
void	setup_pipeline_signals(void);
void	execute_pipe_child_with_redirections(t_split cmd,
			t_pipe_child_context *ctx, t_shell *shell);
int		has_parentheses_in_split(t_split split);
int		check_pipe_error(t_split split, int i, int has_parentheses);
int		check_redirection_error(t_split split, int i);
char	*remove_quotes_for_redirection(char *str);
int		create_pipes_array(int ***pipes, int cmd_count);
void	cleanup_pipes(int **pipes, int cmd_count);
void	close_all_pipes(int **pipes, int cmd_count);
void	setup_pipe_fds(t_pipe_setup_context *ctx);
int		create_pipeline_pipes(int ***pipes, int cmd_count);
void	cleanup_pipeline_pipes(int **pipes, int cmd_count);
int		wait_for_pipeline_processes(pid_t *pids, int cmd_count);
void	setup_child_redirection(t_pipe_context *ctx);
int		setup_pipeline_resources(t_split **commands, int ***pipes, pid_t **pids,
			int cmd_count);
int		execute_pipeline_children(t_pipeline_context *pipeline_ctx);
void	cleanup_pipeline_resources(t_split *commands, int **pipes, pid_t *pids,
			int cmd_count);

/*
** ────────────────────────────────────────────────────────────────────────────
**                          I/O REDIRECTION SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Comprehensive input/output redirection with file and pipe support
*/
int		is_redirection(char *token);
int		has_redirections(t_split split);
int		validate_redirection_syntax(t_split split);
char	*remove_quotes_for_redirection(char *str);
int		handle_here_doc(char *delimiter, int *pipe_fd);
char	**parse_redirections(t_split split, t_redir_fds *fds, t_shell *shell);
int		execute_with_redirections(t_split split, t_shell *shell);
int		process_single_redirection(char **args, int i, t_redir_fds *fds,
			t_shell *shell);
void	redirection_fail_procedure(t_redir_fds *fds);
int		handle_here_document(char *processed_filename, t_redir_fds *fds);
int		handle_output_redirect(char *processed_filename, t_redir_fds *fds,
			int redirect_type);
char	**build_clean_args(char **args, int clean_count);
int		count_clean_args(char **args);
void	print_pipe_error(void);
void	print_newline_error(void);
void	print_token_error(char *token);
int		check_pipe_error(t_split split, int i, int has_parentheses);
int		check_redirection_error(t_split split, int i);

/*
** ────────────────────────────────────────────────────────────────────────────
**                            QUOTE PROCESSING SYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Advanced quote handling with variable expansion and escape processing
*/
int		check_quotes(char *str);
char	*process_quotes(char *str, t_shell *shell);
void	process_args_quotes(char **args, t_shell *shell);
char	*expand_variables_quoted(char *str, t_shell *shell);
char	*remove_quotes_from_string(char *str);
int		is_entirely_single_quoted(char *str);

/*
** ────────────────────────────────────────────────────────────────────────────
**                              UTILITY FUNCTIONS
** ────────────────────────────────────────────────────────────────────────────
** General-purpose utility functions for common operations
*/
char	**split_to_args(t_split split);
char	**filter_empty_args(char **args);
char	*revert_split_str(t_split split);
void	free_args(char **args);
int		is_empty_or_whitespace(char *str);
int		count_non_empty_args(char **args);
int		copy_non_empty_args(char **args, char **filtered);
void	process_and_check_args(char **args, t_shell *shell);
void	handle_empty_pipe_args(char **args);
char	*find_executable(char *cmd, t_shell *shell);
void	compact_args(char **args);
char	*wildcard_input_modify(char *current_input, t_shell *shell);
int		paranthesis_parity_check(t_split split);
int		check_single_par(t_split split);
int		validate_and_process_args(char **args, t_shell *shell);
int		validate_executable(char *cmd, char *executable);
int		handle_executable_not_found(char **args);
int		execute_builtin_with_redirect(t_redir_exec_context *ctx);
int		execute_external_with_redirect(t_redir_exec_context *ctx);
int		handle_empty_args(char **args, int input_fd, int output_fd,
			int stderr_fd);
void	process_variable_expansion(char **args, t_shell *shell);
int		check_empty_command_after_expansion(char **args, int input_fd,
			int output_fd, int stderr_fd);

#endif