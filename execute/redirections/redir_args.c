/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:30:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	close_redirect_fds(int input_fd, int output_fd, int stderr_fd)
{
	if (input_fd != STDIN_FILENO)
		close(input_fd);
	if (output_fd != STDOUT_FILENO)
		close(output_fd);
	if (stderr_fd != STDERR_FILENO)
		close(stderr_fd);
}

int	handle_empty_args(char **args, int input_fd, int output_fd, int stderr_fd)
{
	if (!args[0])
	{
		free_args(args);
		close_redirect_fds(input_fd, output_fd, stderr_fd);
		return (0);
	}
	return (-1);
}

void	process_variable_expansion(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	i = 0;
	while (args[i])
	{
		expanded = expand_variables(args[i], shell);
		if (expanded && expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}
	process_args_quotes(args, shell);
}

int	check_empty_command_after_expansion(char **args, int input_fd,
										int output_fd, int stderr_fd)
{
	if (!args[0])
	{
		close_redirect_fds(input_fd, output_fd, stderr_fd);
		free_args(args);
		return (0);
	}
	if (args[0][0] == '\0')
	{
		write(STDERR_FILENO, ": command not found\n", 20);
		free_args(args);
		return (127);
	}
	return (-1);
}
