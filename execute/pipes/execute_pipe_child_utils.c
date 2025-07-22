/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_child_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:25:13 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 21:30:44 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

char	**execute_expanded_args_split(char *reconstructed, char **args,
		t_shell *shell)
{
	char	*wildcard_expanded;
	t_split	new_split;

	wildcard_expanded = wildcard_input_modify(reconstructed);
	if (wildcard_expanded && wildcard_expanded != reconstructed)
		reconstructed = wildcard_expanded;
	new_split = create_split_str(reconstructed);
	if (new_split.size == 0)
	{
		free_split(&new_split);
		process_args_quotes(args, shell);
		return (args);
	}
	free_args(args);
	args = split_to_args(new_split);
	free_split(&new_split);
	if (args)
		process_args_quotes(args, shell);
	return (args);
}

void	write_pipe_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

void	execute_pipe_external_command(char **args, t_shell *shell)
{
	char	*executable;

	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write_pipe_error_message(args[0], ": command not found\n");
		exit(127);
	}
	execve(executable, args, shell->envp);
	perror("execve");
	exit(127);
}
