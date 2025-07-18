/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_refactored.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 22:05:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/18 21:41:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_new.h"

static void	process_input(t_shell *shell)
{
	if (!check_quotes(shell->current_input))
	{
		write(STDERR_FILENO, "Error: unclosed quotes\n", 23);
		shell->past_exit_status = 2;
		return;
	}
	
	// Simple processing without wildcard for now
	shell->split_input = create_split_str(shell->current_input);
	
	if (shell->split_input.size > 0)
		shell->past_exit_status = execute_command(shell->split_input, shell);
	
	free_split(&(shell->split_input));
}

static void	shell_loop(t_shell *shell)
{
	setup_signals();
	
	while (!shell->should_exit)
	{
		g_signal = 0;
		shell->current_input = readline("minishell > ");
		
		if (!shell->current_input)
			safe_exit(shell);
		
		if (g_signal == SIGINT)
		{
			shell->past_exit_status = 130;
			g_signal = 0;
		}
		
		add_history(shell->current_input);
		
		if (!is_empty(shell->current_input))
			process_input(shell);
		
		free(shell->current_input);
	}
	
	free_environment(shell);
	rl_clear_history();
	exit(shell->exit_code);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	
	shell_init(&shell);
	init_environment(&shell, envp);
	shell_loop(&shell);
	
	return (0);
}
