/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:31:37 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:24:37 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include <errno.h>
#include <limits.h>

static int	is_valid_number(char *arg)
{
	int	i;

	i = 0;
	if (arg[i] == '+' || arg[i] == '-')
		i++;
	if (arg[i] == '\0')
		return (0);
	while (arg[i])
	{
		if (!ft_isdigit(arg[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	write_exit_error(char *arg)
{
	write(STDERR_FILENO, "exit: ", 6);
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, ": numeric argument required\n", 28);
}

static int	validate_numeric_arg(char *arg, t_shell *shell)
{
	char		*endptr;

	if (!is_valid_number(arg))
	{
		write_exit_error(arg);
		shell->should_exit = 1;
		shell->exit_code = 2;
		return (0);
	}
	errno = 0;
	strtoll(arg, &endptr, 10);
	if (errno == ERANGE || *endptr != '\0')
	{
		write_exit_error(arg);
		shell->should_exit = 1;
		shell->exit_code = 2;
		return (0);
	}
	return (1);
}

int	builtin_exit(char **args, t_shell *shell)
{
	int	exit_code;

	if (args[1] && args[2])
	{
		write(STDERR_FILENO, "exit: too many arguments\n", 25);
		return (1);
	}
	if (args[1])
	{
		if (!validate_numeric_arg(args[1], shell))
			return (shell->exit_code);
		exit_code = ft_atoi(args[1]) & 255;
	}
	else
		exit_code = shell->past_exit_status;
	shell->should_exit = 1;
	shell->exit_code = exit_code;
	return (exit_code);
}
