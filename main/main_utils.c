/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:29:14 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/23 17:56:24 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_empty(char *str)
{
	int	i;

	if (str == NULL)
		return (1);
	i = 0;
	while (str[i] != '\0')
	{
		if (!(str[i] == ' ' || (str[i] <= 13 && str[i] >= 9)))
			return (0);
		i++;
	}
	return (1);
}

void	safe_exit(t_shell *shell)
{
	if (shell->current_input != NULL)
		free(shell->current_input);
	if (shell->split_input.start != NULL)
		free_split(&(shell->split_input));
	free_environment(shell);
	rl_clear_history();
	exit(shell->past_exit_status);
}

void	shell_init(t_shell *shell)
{
	shell->current_input = NULL;
	shell->split_input = create_split(NULL, 0);
	shell->past_exit_status = 0;
	shell->should_exit = 0;
	shell->exit_code = 0;
	shell->in_subshell = 0;
	shell->envp = NULL;
	shell->prompt = NULL;
	shell->terminal_prompt = "minishell > ";
}
