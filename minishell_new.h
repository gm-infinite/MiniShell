/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_new.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 21:55:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/18 21:41:43 by emgenc           ###   ########.fr       */
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
# include "e-libft/libft.h"
# include "parser/t_split_utils/t_split.h"

# ifndef PATH_MAX
#  define PATH_MAX 4096
# endif

extern volatile sig_atomic_t g_signal;

typedef struct s_shell
{
	int		past_exit_status;
	char	*current_input;
	t_split	split_input;
	char	**envp;
	int		should_exit;
	int		exit_code;
}			t_shell;

// Core functions
void	shell_init(t_shell *shell);
void	safe_exit(t_shell *shell);
int		is_empty(char *str);

// Signal handling
void	setup_signals(void);
void	signal_handler(int sig);
void	signal_handler_child(int sig);
void	setup_child_signals(void);

// Parser functions (keep existing)
void	parser_and_or(t_shell *shell, t_split split);
int		countchr_str(char *str, char c);
int		countchr_quote(char *str, char c);
int		countchr_not_quote(char *str, char c);
int		check_single_par(t_split split);
int		check_symbol(t_split split, char *find, int flag);
void	sep_opt_arg(t_shell *shell);
int		count_str_split(t_split split, const char *str, int flag);
void	cut_out_par(t_split *split);

// Environment
char	*expand_variables(char *str, t_shell *shell);
char	*get_env_value(char *var_name, t_shell *shell);
void	init_environment(t_shell *shell, char **envp);
void	free_environment(t_shell *shell);
int		set_env_var(char *var_name, char *value, t_shell *shell);
int		unset_env_var(char *var_name, t_shell *shell);

// Builtins
int		is_builtin(char *cmd);
int		execute_builtin(char **args, t_shell *shell);

// Execution
int		execute_command(t_split split, t_shell *shell);
int		execute_pipeline(t_split split, t_shell *shell);
int		execute_single_command(char **args, t_shell *shell);
t_split	process_parentheses_in_split(t_split cmd, t_shell *shell);

// Redirections
int		is_redirection(char *token);
int		has_redirections(t_split split);
int		handle_here_doc(char *delimiter, int *pipe_fd);
char	**parse_redirections(t_split split, int *input_fd, int *output_fd, int *stderr_fd);
int		execute_with_redirections(t_split split, t_shell *shell);

// Quote processing (keep existing)
int		check_quotes(char *str);
char	*process_quotes(char *str, t_shell *shell);
void	process_args_quotes(char **args, t_shell *shell);
char	*expand_variables_quoted(char *str, t_shell *shell);

// Utilities
char	**split_to_args(t_split split);
void	free_args(char **args);
char	*find_executable(char *cmd, t_shell *shell);
void	compact_args(char **args);
int		count_pipes(t_split split);
t_split	*split_by_pipes(t_split split, int *cmd_count);
char	*wildcard_input_modify(char *current_input, t_shell *shell);
int		paranthesis_parity_check(t_split split);

#endif
