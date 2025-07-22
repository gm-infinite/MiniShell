/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:55:46 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/20 16:15:03 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "get_next_line.h"

static void	begin_command_parsing_and_execution(t_shell *shell)
{
	char	*expanded_input;

	if (!check_quotes(shell->current_input))
		shell->past_exit_status = 2;
	else
	{
		sep_opt_arg(shell);
		expanded_input = wildcard_input_modify(shell->current_input);
		if (expanded_input && expanded_input != shell->current_input)
		{
			free(shell->current_input);
			shell->current_input = expanded_input;
		}
		shell->split_input = create_split_str(shell->current_input);
		if (shell->split_input.size > 0)
			parser_and_or(shell, shell->split_input);
		free_split(&(shell->split_input));
	}
}

static void	start_shell(t_shell *shell)
{
	setup_signals();
	while (!shell->should_exit)
	{
		g_signal = 0;
		if (isatty(fileno(stdin)))
			shell->prompt = readline(shell->terminal_prompt);
		else
		{
			char	*line;

			line = get_next_line(fileno(stdin));
			if (line)
			{
				shell->prompt = ft_strtrim(line, "\n");
				free(line);
			}
			else
				shell->prompt = NULL;
		}
		shell->current_input = shell->prompt;
		if (shell->current_input == NULL)
			safe_exit(shell);
		if (g_signal == SIGINT)
		{
			shell->past_exit_status = 130;
			g_signal = 0;
		}
		add_history(shell->current_input);
		if (!is_empty(shell->current_input))
			begin_command_parsing_and_execution(shell);
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
	start_shell(&shell);
	return (0);
}
