/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:11:14 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 12:41:09 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

static void	init_env(t_shell *shell, char **envp)
{
	int	i;
	int	count;

	count = 0;
	while (envp[count])
		count++;
	shell->envp = malloc(sizeof(char *) * (count + 1));
	if (!shell->envp)
		return ;
	i = 0;
	while (i < count)
	{
		shell->envp[i] = ft_strdup(envp[i]);
		if (!shell->envp[i])
		{
			while (--i >= 0)
				free(shell->envp[i]);
			free(shell->envp);
			shell->envp = NULL;
			return ;
		}
		i++;
	}
	shell->envp[i] = NULL;
}

static int	check_quotes(char *str)
{
	int	i;
	int	single_open;
	int	double_open;

	if (!str)
		return (1);
	i = -1;
	single_open = 0;
	double_open = 0;
	while (str[++i])
	{
		if (str[i] == '\'' && !double_open)
			single_open = !single_open;
		else if (str[i] == '"' && !single_open)
			double_open = !double_open;
	}
	return (single_open == 0 && double_open == 0);
}

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
		else if (expanded_input && expanded_input != shell->current_input)
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
		shell->prompt = readline(shell->terminal_prompt);
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
	free_env(shell);
	rl_clear_history();
	exit(shell->exit_code);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	shell_init(&shell);
	init_env(&shell, envp);
	start_shell(&shell);
	return (0);
}
