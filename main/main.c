/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:55:46 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/03/22 15:45:42 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_result(t_shell *shell)
{
	int	i;

	printf("\n\tseperated:%s", shell->current_input);
	printf("\n\tsize: %d", shell->split_input.size);
	printf("\n\tstart:%3p", shell->split_input.start);
	i = 0;
	while(i <= shell->split_input.size)
	{
		printf("\n\t\t%-2d:%s", i, shell->split_input.start[i]);
		i++;
	}
	printf(" -> after last value\n\n");
}

//initiates shell struct, signal's and start of readline sequence
static void	start_shell(t_shell *shell)
{
	// We need to handle signals at some point.
	while (1)
	{
		shell->current_input = readline("minishell > ");
		if (shell->current_input == NULL)
			safe_exit(shell);
		if (!is_empty(shell->current_input))
		{
			add_history(shell->current_input);
			sep_opt_arg(shell);
			shell->split_input = create_split_str(shell->current_input);
			print_result(shell);
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
