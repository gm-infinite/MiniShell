/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:31:37 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	builtin_exit(char **args, t_shell *shell)
{
	int	exit_code;

	printf("exit\n");
	if (args[1] && args[2])
	{
		write(STDERR_FILENO, "exit: too many arguments\n", 25);
		return (1);
	}
	if (args[1])
	{
		exit_code = ft_atoi(args[1]);
		// Check if it's a valid number
		if (!ft_isdigit(args[1][0]) && args[1][0] != '-' && args[1][0] != '+')
		{
			write(STDERR_FILENO, "exit: numeric argument required\n", 32);
			exit_code = 2;
		}
	}
	else
		exit_code = shell->past_exit_status;
	// Set exit flag instead of calling exit() directly
	shell->should_exit = 1;
	shell->exit_code = exit_code;
	return (exit_code);
}
