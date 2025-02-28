/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:55:46 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/02/13 14:26:00 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//safe exit for ctrl + D
//(might be used later for other stuff)
void	safe_exit(t_shell *shell)
{
	if (shell->current_input != NULL)
		free(shell->current_input);
	if (shell->split_input.start != NULL)
		free_split(&(shell->split_input));
	rl_clear_history();
	exit(0);
}

//initiates shell struct, signal's and start of readline sequence
static void	start_shell(t_shell *shell)
{
	//we need to handle signals at some point
	while (1)
	{
		shell->current_input = readline("minishell > ");
		if (shell->current_input == NULL)
			safe_exit(shell);
		if (!is_empty(shell->current_input))
		{
			add_history(shell->current_input);
			shell->split_input = create_split_str(shell->current_input);
			parser_and_or(shell, shell->split_input);
			free_split(&(shell->split_input));
		}
		free(shell->current_input);
	}
}

//this is to stop some uninitilized values being used in if statements.
//if shell struct grows please put a defult initilize here.
void	shell_init(t_shell *shell)
{
	shell->child_pid = -1;
	shell->current_input = NULL;
	shell->past_exit_status = 0;
	shell->split_input = create_split(NULL, 0);
}

int	main(void)
{
	t_shell	shell;

	shell_init(&shell);
	start_shell(&shell);
}
