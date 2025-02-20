/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:53:30 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/02/13 14:27:14 by kuzyilma         ###   ########.fr       */
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
# include "../t_split_utils/t_split.h"

// ❍︎✋︎☠︎♓︎⬧︎♒︎♏︎●︎☹︎ !!

typedef struct s_shell
{
	int		past_exit_status;
	char	*current_input;
	t_split	split_input;
}			t_shell;

void	safe_exit(t_shell *shell);
int		is_empty(char *str);

#endif