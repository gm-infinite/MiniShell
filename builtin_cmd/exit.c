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

static int	validate_numeric_arg(char *arg, t_shell *shell)
{
	if (!is_valid_number(arg))
	{
		fprintf(stderr, "exit: %s: numeric argument required\n", arg);
		shell->should_exit = 1;
		shell->exit_code = 2;
		return (0);
	}
	return (1);
}

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
		if (!validate_numeric_arg(args[1], shell))
			return (2);
		exit_code = ft_atoi(args[1]);
	}
	else
		exit_code = shell->past_exit_status;
	shell->should_exit = 1;
	shell->exit_code = exit_code;
	return (exit_code);
}
