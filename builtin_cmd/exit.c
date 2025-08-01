/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:16:52 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 22:15:02 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include <errno.h>
#include <limits.h>

static long long	ft_strtoll(char *str, char **endptr, int base)
{
	long long	result;
	int			sign;
	int			digit;

	result = 0;
	sign = 1;
	if (*str == '-' || *str == '+')
		if (*str++ == '-')
			sign = -1;
	while (*str && ft_isdigit(*str))
	{
		digit = *str++ - '0';
		if (result > (LLONG_MAX - digit) / base)
		{
			errno = ERANGE;
			if (sign == 1)
				return (LLONG_MAX);
			return (LLONG_MIN);
		}
		result = result * base + digit;
	}
	if (endptr)
		*endptr = str;
	return (result * sign);
}

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
	char		*endptr;

	if (!is_valid_number(arg))
	{
		write(STDERR_FILENO, "exit: ", 6);
		write(STDERR_FILENO, arg, ft_strlen(arg));
		write(STDERR_FILENO, ": numeric argument required\n", 28);
		shell->should_exit = 1;
		shell->exit_code = 2;
		return (0);
	}
	errno = 0;
	ft_strtoll(arg, &endptr, 10);
	if (errno == ERANGE || *endptr != '\0')
	{
		write(STDERR_FILENO, "exit: ", 6);
		write(STDERR_FILENO, arg, ft_strlen(arg));
		write(STDERR_FILENO, ": numeric argument required\n", 28);
		shell->should_exit = 1;
		shell->exit_code = 2;
		return (0);
	}
	return (1);
}

int	builtin_exit(char **args, t_shell *shell)
{
	int	exit_code;
	int	already_printed;

	already_printed = 0;
	if (args[1])
	{
		if (!validate_numeric_arg(args[1], shell))
			return (shell->exit_code);
		if (args[2])
		{
			write(STDERR_FILENO, "exit\n", 5);
			write(STDERR_FILENO, "exit: too many arguments\n", 25);
			already_printed = 1;
		}
		exit_code = ft_atoi(args[1]) & 255;
	}
	else
		exit_code = shell->past_exit_status;
	shell->should_exit = !already_printed;
	if (already_printed == 1)
		exit_code = 1;
	shell->exit_code = exit_code;
	if (!already_printed)
		printf("exit\n");
	return (exit_code);
}
