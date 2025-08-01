/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getters.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:44:37 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 16:03:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

t_pipe_setup_ctx	get_pipe_ctx(int cmd_index, int cmd_count,
	int **pipes, int *fd_values)
{
	t_pipe_setup_ctx	ret;

	ret.cmd_count = cmd_count;
	ret.cmd_index = cmd_index;
	ret.pipes = pipes;
	ret.input_fd = &(fd_values[0]);
	ret.output_fd = &(fd_values[1]);
	return (ret);
}

t_pipe_cleaner	get_cleanup(t_split *commands, int **pipes,
		pid_t *pids, int cmd_count)
{
	t_pipe_cleaner	ret;

	ret.commands = commands;
	ret.pipes = pipes;
	ret.pids = pids;
	ret.cmd_count = cmd_count;
	return (ret);
}

t_redir_fds	get_fds(int *fd_values)
{
	t_redir_fds	ret;

	ret.input_fd = &fd_values[0];
	ret.output_fd = &fd_values[1];
	ret.stderr_fd = &fd_values[2];
	ret.preprocessed_heredoc = 0;
	return (ret);
}
