/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_process_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 14:48:04 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int	heredoc_subprocess(t_heredoc_sub heresub, char *delim, t_pipeline_context *pipeline_ctx, char *temp_filename)
{
	pid_t	pid;
	int		status;
	int		i;

	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		rl_catch_signals = 0;
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_IGN);
		process_heredoc_content(heresub.temp_fd, delim, pipeline_ctx->shell, heresub.should_expand);
		free(delim);
		free_split(&(pipeline_ctx->shell->split_input));
		free_environment(pipeline_ctx->shell);
		free(temp_filename);
		free(pipeline_ctx->shell->current_input);
		free_args(heresub.args);
		free(pipeline_ctx->commands);
		i = -1;
		while (++i < pipeline_ctx->cmd_count - 1)
			if (pipeline_ctx->pipes[i])
				free(pipeline_ctx->pipes[i]);
		free(pipeline_ctx->pids);
		free(pipeline_ctx->pipes);
		_exit(EXIT_SUCCESS);
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		unlink(temp_filename);
		return (1);
	}
	return (0);
}

static int	setup_heredoc_file(char **processed_delimiter, char **temp_filename,
		int cmd_index, char *delimiter)
{
	int	temp_fd;

	*processed_delimiter = process_heredoc_delimiter(delimiter);
	if (!*processed_delimiter)
		return (-1);
	*temp_filename = create_heredoc_filename(cmd_index);
	if (!*temp_filename)
	{
		free(*processed_delimiter);
		return (-1);
	}
	temp_fd = open(*temp_filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (temp_fd == -1)
	{
		free(*processed_delimiter);
		free(*temp_filename);
		return (-1);
	}
	return (temp_fd);
}

static void	update_command_filename(t_pipeline_context *pipeline_ctx, int i,
		int j, char *temp_filename)
{
	int	split_pos;

	split_pos = j + 1;
	if (split_pos < pipeline_ctx->commands[i].size)
	{
		free(pipeline_ctx->commands[i].start[split_pos]);
		pipeline_ctx->commands[i].start[split_pos] = temp_filename;
	}
	else
		free(temp_filename);
}

int	handle_heredoc_redirection(char **args, int j,
		t_pipeline_context *pipeline_ctx, int i)
{
	char			*processed_delimiter;
	t_heredoc_sub	heresub;
	char			*temp_filename;

	heresub.args = args;
	heresub.temp_fd = setup_heredoc_file(&processed_delimiter, &temp_filename,
			i, args[j + 1]);
	if (heresub.temp_fd == -1)
		return (1);
	heresub.should_expand = !delimiter_was_quoted(args[j + 1]);
	if (heredoc_subprocess(heresub, processed_delimiter, pipeline_ctx,
			temp_filename) != 0)
	{
		close(heresub.temp_fd);
		return (1);
	}
	close(heresub.temp_fd);
	update_command_filename(pipeline_ctx, i, j, temp_filename);
	free(processed_delimiter);
	return (0);
}

int	process_command_redirections(char **args,
		t_pipeline_context *pipeline_ctx, int i)
{
	int	j;

	j = 0;
	while (args[j])
	{
		if (is_redirection(args[j]) == 1)
		{
			if (!args[j + 1])
			{
				write(STDERR_FILENO, "syntax error: missing filename\n", 32);
				return (1);
			}
			if (handle_heredoc_redirection(args, j, pipeline_ctx, i) != 0)
				return (1);
			j += 2;
		}
		else
		{
			if (is_redirection(args[j]))
				j += 2;
			else
				j++;
		}
	}
	return (0);
}
