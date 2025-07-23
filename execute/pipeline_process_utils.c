/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_process_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 11:34:19 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

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
	char	*processed_delimiter;
	int		temp_fd;
	char	*temp_filename;
	int		should_expand;

	temp_fd = setup_heredoc_file(&processed_delimiter, &temp_filename,
			i, args[j + 1]);
	if (temp_fd == -1)
		return (1);
	should_expand = !delimiter_was_quoted(args[j + 1]);
	process_heredoc_content(temp_fd, processed_delimiter, 
		pipeline_ctx->shell, should_expand);
	close(temp_fd);
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
