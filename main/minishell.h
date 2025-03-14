/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/03/14 15:18:15 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <readline/readline.h>
# include <readline/history.h>
# define _POSIX_C_SOURCE 200809L
# include <signal.h>
# include <sys/wait.h>
# include "../t_split_utils/t_split.h"

// ❍︎✋︎☠︎♓︎⬧︎♒︎♏︎●︎☹︎ !!

typedef struct s_shell
{
	int		past_exit_status;
	char	*current_input;
	t_split	split_input;
	pid_t	child_pid;
}			t_shell;

void	safe_exit(t_shell *shell);
int		is_empty(char *str);

void	parser_and_or(t_shell *shell, t_split split);
int		countchr_str(char*str, char c);
int		countchr_quote(char*str, char c);
int		countchr_not_quote(char*str, char c);
int		check_single_par(t_split split);
int		check_symbol(t_split split, char *find, int flag);
void	echo(t_split split);
#endif