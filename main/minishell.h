/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
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
void	sep_opt_arg(t_shell *shell);
int		count_str_split(t_split split, const char *str, int flag);
void	cut_out_par(t_split *split);
int		check_operator_syntax_errors(t_split split);
int		check_parentheses_syntax_errors(t_split split);

/*
** ────────────────────────────────────────────────────────────────────────────
**                      ENVIRONMENT VARIABLE SUBSYSTEM
** ────────────────────────────────────────────────────────────────────────────
** Dynamic environment management with variable expansion and modification support
*/
char	*expand_variables(char *str, t_shell *shell);
char	*expand_tilde(char *str, t_shell *shell);
char	*get_env_value(char *var_name, t_shell *shell);
void	init_environment(t_shell *shell, char **envp);
void	free_environment(t_shell *shell);
int		set_env_var(char *var_name, char *value, t_shell *shell);
int		unset_env_var(char *var_name, t_shell *shell);

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
int		builtin_pwd(void);
int		builtin_export(char **args, t_shell *shell);
int		builtin_unset(char **args, t_shell *shell);
int		builtin_env(t_shell *shell);
int		builtin_exit(char **args, t_shell *shell);
int		set_env_var(char *var_name, char *value, t_shell *shell);
int		append_env_var(char *var_name, char *value, t_shell *shell);
int		print_sorted_env(t_shell *shell);
int		process_export_args(char **args, t_shell *shell);

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
int		execute_pipe_command(t_split cmd, int input_fd, int output_fd, t_shell *shell, int **all_pipes, int pipe_count);
void	execute_pipe_child(t_split cmd, int cmd_index, int **pipes, int cmd_count, t_shell *shell);
void	execute_pipe_child_with_redirections(t_split cmd, int cmd_index, int **pipes, int cmd_count, t_shell *shell);
int		has_parentheses_in_split(t_split split);
int		check_pipe_error(t_split split, int i, int has_parentheses);
int		check_redirection_error(t_split split, int i);
char	*remove_quotes_for_redirection(char *str);

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
char	**parse_redirections(t_split split, int *input_fd, int *output_fd, int *stderr_fd, t_shell *shell);
int		execute_with_redirections(t_split split, t_shell *shell);
int		process_single_redirection(char **args, int i, int *input_fd,
			int *output_fd, int *stderr_fd, t_shell *shell);
char	**build_clean_args(char **args, int clean_count);
int		count_clean_args(char **args);

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
char	*find_executable(char *cmd, t_shell *shell);
void	compact_args(char **args);
char	*wildcard_input_modify(char *current_input, t_shell *shell);
int		paranthesis_parity_check(t_split split);
int		check_single_par(t_split split);

#endif