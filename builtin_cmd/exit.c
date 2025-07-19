/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:31:37 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/19 08:07:33 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	builtin_exit(char **args, t_shell *shell)
{
	int	exit_code;
	int	i;

	printf("exit\n");
	if (args[1] && args[2])
	{
		write(STDERR_FILENO, "exit: too many arguments\n", 25);
		return (1);
	}
	if (args[1])
	{
		// Enhanced numeric validation
		i = 0;
		if (args[1][i] == '+' || args[1][i] == '-')
			i++;
		
		// Check if remaining characters are all digits
		if (args[1][i] == '\0')  // Just + or - alone
		{
			write(STDERR_FILENO, "exit: ", 6);
			write(STDERR_FILENO, args[1], ft_strlen(args[1]));
			write(STDERR_FILENO, ": numeric argument required\n", 28);
			shell->should_exit = 1;
			shell->exit_code = 2;
			return (2);
		}
		
		while (args[1][i])
		{
			if (!ft_isdigit(args[1][i]))
			{
				write(STDERR_FILENO, "exit: ", 6);
				write(STDERR_FILENO, args[1], ft_strlen(args[1]));
				write(STDERR_FILENO, ": numeric argument required\n", 28);
				shell->should_exit = 1;
				shell->exit_code = 2;
				return (2);
			}
			i++;
		}
		
		exit_code = ft_atoi(args[1]);
	}
	else
		exit_code = shell->past_exit_status;
	// Set exit flag instead of calling exit() directly
	shell->should_exit = 1;
	shell->exit_code = exit_code;
	return (exit_code);
}
