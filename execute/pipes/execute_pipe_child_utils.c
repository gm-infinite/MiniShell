/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_child_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 22:25:13 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 17:43:28 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

char	**execute_expanded_args_split(char *reconstructed, char **args,
		t_shell *shell)
{
	char	*wildcard_expanded;
	char	*to_process;
	t_split	new_split;

	wildcard_expanded = wildcard_input_modify(reconstructed);
	if (wildcard_expanded && wildcard_expanded != reconstructed)
		to_process = wildcard_expanded;
	else
		to_process = reconstructed;
	new_split = create_split_str(to_process);
	if (new_split.size == 0)
	{
		free_split(&new_split);
		if (wildcard_expanded && wildcard_expanded != reconstructed)
			free(wildcard_expanded);
		process_args_quotes(args, shell);
		return (args);
	}
	free_args(args);
	args = split_to_args(new_split);
	free_split(&new_split);
	if (wildcard_expanded && wildcard_expanded != reconstructed)
		free(wildcard_expanded);
	if (args)
		process_args_quotes(args, shell);
	return (args);
}

void	write_pipe_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

void	execute_pipe_external_command(char **args, t_shell *shell, t_split *commands,
	int **pipes, pid_t *pids, int cmd_count)
{
	char	*executable;

	executable = find_executable(args[0], shell);
	if (!executable)
	{
		write_pipe_error_message(args[0], ": command not found\n");
		free_child_pipeline_memory(args, shell, commands, pipes, pids, cmd_count);
		exit(127);
	}
	execve(executable, args, shell->envp);
	perror("execve");
	free_child_pipeline_memory(args, shell, commands, pipes, pids, cmd_count);
	exit(127);
}
