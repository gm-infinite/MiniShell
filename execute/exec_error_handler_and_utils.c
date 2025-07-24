/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_error_handler_and_utils.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 21:08:18 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/24 21:21:42 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

t_pipe_setup_context	get_pipe_ctx(int cmd_index, int cmd_count,
	int **pipes, int *fd_values)
{
	t_pipe_setup_context	ret;

	ret.cmd_count = cmd_count;
	ret.cmd_index = cmd_index;
	ret.pipes = pipes;
	ret.input_fd = &(fd_values[0]);
	ret.output_fd = &(fd_values[1]);
	return (ret);
}

t_pipeline_cleanup	get_cleanup(t_split *commands, int **pipes,
		pid_t *pids, int cmd_count)
{
	t_pipeline_cleanup	ret;

	ret.commands = commands;
	ret.pipes = pipes;
	ret.pids = pids;
	ret.cmd_count = cmd_count;
	return (ret);
}

void	write_exec_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

void	handle_executable_error_and_exit(char **args, char *executable,
		t_shell *shell)
{
	if (!executable)
	{
		write_exec_error_message(args[0], ": command not found\n");
		free_child_memory(args, shell);
		exit(127);
	}
	if (ft_strncmp(executable, "__NOT_DIRECTORY__", 17) == 0)
	{
		write_not_directory_error(args[0]);
		free(executable);
		free_child_memory(args, shell);
		exit(126);
	}
}

void	handle_pipeline_executable_error(char **args, char *executable,
		t_shell *shell, t_pipeline_cleanup *cleanup)
{
	if (!executable)
	{
		write_exec_error_message(args[0], ": command not found\n");
		free_child_pipeline_memory(args, shell, cleanup);
		exit(127);
	}
	if (ft_strncmp(executable, "__NOT_DIRECTORY__", 17) == 0)
	{
		write_not_directory_error(args[0]);
		free(executable);
		free_child_pipeline_memory(args, shell, cleanup);
		exit(126);
	}
}
