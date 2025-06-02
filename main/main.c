/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:55:46 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/06/02 14:04:30 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//initiates shell struct, signal's and start of readline sequence
static void	start_shell(t_shell *shell)
{
	// We need to handle signals at some point.
	while (1)
	{
		shell->current_input = readline("minishell > ");
		if (shell->current_input == NULL)
			safe_exit(shell);
		add_history(shell->current_input);
		if (!is_empty(shell->current_input))
		{
			sep_opt_arg(shell);
			shell->split_input = create_split_str(shell->current_input);
			parser_and_or(shell, shell->split_input);
			free_split(&(shell->split_input));
		}
		free(shell->current_input);
	}
}

// This is to stop some uninitilized values being used in if statements.
// If shell struct grows, please put a defult initilize here.
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
