/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 20:17:42 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../main/get_next_line.h"

static int	wait_for_children(pid_t *pids, int cmd_count)
{
	int	status;
	int	exit_status;
	int	i;

	exit_status = 0;
	i = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status))
			exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			exit_status = 128 + WTERMSIG(status);
			if (WTERMSIG(status) == SIGINT)
				write(1, "\n", 1);
		}
		i++;
	}
	return (exit_status);
}

static int	preprocess_heredocs_in_pipeline(t_pipeline_context *pipeline_ctx)
{
	int		i;
	int		j;
	char	**args;
	
	// Process heredocs sequentially for each command in the pipeline
	i = 0;
	while (i < pipeline_ctx->cmd_count)
	{
		args = split_to_args(pipeline_ctx->commands[i]);
		if (!args)
		{
			i++;
			continue;
		}
		
		// Look for heredoc redirections in this command
		j = 0;
		while (args[j])
		{
			if (is_redirection(args[j]) == 1) // heredoc redirection
			{
				if (!args[j + 1])
				{
					write(STDERR_FILENO, "syntax error: missing filename\n", 32);
					free_args(args);
					return (1);
				}
				
				// Process heredoc delimiter
				char *processed_delimiter = process_heredoc_delimiter(args[j + 1]);
				if (!processed_delimiter)
				{
					free_args(args);
					return (1);
				}
				
				// Create a temporary file to store heredoc content
				char temp_filename[64];
				snprintf(temp_filename, sizeof(temp_filename), "/tmp/.minishell_heredoc_%d_%d", getpid(), i);
				
				int temp_fd = open(temp_filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
				if (temp_fd == -1)
				{
					free(processed_delimiter);
					free_args(args);
					return (1);
				}
				
				// Collect heredoc input and write to temp file
				char *line;
				while (1)
				{
					if (isatty(fileno(stdin)))
						line = readline("> ");
					else
					{
						line = get_next_line(fileno(stdin));
						if (line)
						{
							char *trimmed = ft_strtrim(line, "\n");
							free(line);
							line = trimmed;
						}
					}
					
					if (!line)
					{
						// EOF encountered - show warning
						write(STDERR_FILENO, "minishell: warning: here-document delimited by end-of-file (wanted `", 68);
						write(STDERR_FILENO, processed_delimiter, ft_strlen(processed_delimiter));
						write(STDERR_FILENO, "')\n", 3);
						break;
					}
					
					if (ft_strncmp(line, processed_delimiter, ft_strlen(processed_delimiter)) == 0
						&& ft_strlen(line) == ft_strlen(processed_delimiter))
					{
						free(line);
						break;
					}
					
					write(temp_fd, line, ft_strlen(line));
					write(temp_fd, "\n", 1);
					free(line);
				}
				
				close(temp_fd);
				
				// Update the original t_split structure
				// Find the corresponding position in the t_split
				int split_pos = j + 1;
				if (split_pos < pipeline_ctx->commands[i].size)
				{
					free(pipeline_ctx->commands[i].start[split_pos]);
					pipeline_ctx->commands[i].start[split_pos] = ft_strdup(temp_filename);
				}
				
				free(processed_delimiter);
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
		
		free_args(args);
		i++;
	}
	return (0);
}

int	setup_pipeline_resources(t_split **commands, int ***pipes, pid_t **pids,
							int cmd_count)
{
	if (!create_pipes_array(pipes, cmd_count))
	{
		free(*commands);
		return (0);
	}
	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		cleanup_pipes(*pipes, cmd_count);
		free(*commands);
		return (0);
	}
	return (1);
}

int	execute_pipeline_children(t_pipeline_context *pipeline_ctx)
{
	int						i;
	t_pipe_child_context	ctx;

	// Pre-process all heredocs sequentially before forking any children
	if (preprocess_heredocs_in_pipeline(pipeline_ctx) != 0)
		return (1);

	i = 0;
	while (i < pipeline_ctx->cmd_count)
	{
		pipeline_ctx->pids[i] = fork();
		if (pipeline_ctx->pids[i] == -1)
		{
			perror("fork");
			return (1);
		}
		else if (pipeline_ctx->pids[i] == 0)
		{
			ctx.pipes = pipeline_ctx->pipes;
			ctx.cmd_index = i;
			ctx.cmd_count = pipeline_ctx->cmd_count;
			execute_pipe_child_with_redirections(pipeline_ctx->commands[i],
				&ctx, pipeline_ctx->shell);
			exit(127);
		}
		i++;
	}
	close_all_pipes(pipeline_ctx->pipes, pipeline_ctx->cmd_count);
	return (wait_for_children(pipeline_ctx->pids, pipeline_ctx->cmd_count));
}

void	cleanup_pipeline_resources(t_split *commands, int **pipes, pid_t *pids,
								int cmd_count)
{
	cleanup_pipes(pipes, cmd_count);
	free(pids);
	free(commands);
}
