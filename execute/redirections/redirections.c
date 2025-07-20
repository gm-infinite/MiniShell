/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 14:15:09 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void redirection_fail_procedure(int *input_fd, int *output_fd, int *stderr_fd)
{
	if (*input_fd != STDIN_FILENO && *input_fd != -1)
	{
		close(*input_fd);
		*input_fd = STDIN_FILENO;
	}
	if (*output_fd != STDOUT_FILENO && *output_fd != -1)
	{
		close(*output_fd);
		*output_fd = STDOUT_FILENO;
	}
	if (*stderr_fd != STDERR_FILENO && *stderr_fd != -1)
	{
		close(*stderr_fd);
		*stderr_fd = STDERR_FILENO;
	}
}

static int	process_redirections_loop(char **args, int *input_fd, 
							int *output_fd, int *stderr_fd, t_shell *shell)
{
	int	i;

	i = 0;
	while (args[i])
	{
		if (is_redirection(args[i]))
		{
			if (process_single_redirection(args, i, input_fd,
					output_fd, stderr_fd, shell) == -1)
				return (-1);
			i += 2;
		}
		else
			i++;
	}
	return (0);
}

char	**parse_redirections(t_split split, int *input_fd, int *output_fd, int *stderr_fd, t_shell *shell)
{
	char	**args;
	char	**clean_args;
	int		clean_count;

	args = split_to_args(split);
	if (!args)
		return (NULL);
	clean_count = count_clean_args(args);
	clean_args = build_clean_args(args, clean_count);
	if (!clean_args)
	{
		free_args(args);
		return (NULL);
	}
	if (process_redirections_loop(args, input_fd, output_fd, stderr_fd, shell) == -1)
	{
		free_args(args);
		free_args(clean_args);
		redirection_fail_procedure(input_fd, output_fd, stderr_fd);
		return (NULL);
	}
	free_args(args);
	return (clean_args);
}

int	execute_with_redirections(t_split split, t_shell *shell)
{
	char	**args;
	int		input_fd = STDIN_FILENO;
	int		output_fd = STDOUT_FILENO;
	int		stderr_fd = STDERR_FILENO;
	int		result;

	args = parse_redirections(split, &input_fd, &output_fd, &stderr_fd, shell);
	if (!args)
		return (1);
	result = handle_empty_args(args, input_fd, output_fd, stderr_fd);
	if (result != -1)
		return (result);
	process_variable_expansion(args, shell);
	result = check_empty_command_after_expansion(args, input_fd, 
			output_fd, stderr_fd);
	if (result != -1)
		return (result);
	if (is_builtin(args[0]))
		result = execute_builtin_with_redirect(args, input_fd, 
				output_fd, stderr_fd, shell);
	else
		result = execute_external_with_redirect(args, input_fd, 
				output_fd, stderr_fd, shell);
	free_args(args);
	return (result);
}
