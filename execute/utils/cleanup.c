/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:02:48 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:15:22 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void	free_args(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	free_env(t_shell *shell)
{
	int	i;

	if (!shell->envp)
		return ;
	i = 0;
	while (shell->envp[i])
	{
		free(shell->envp[i]);
		i++;
	}
	free(shell->envp);
	shell->envp = NULL;
}

void	free_child_pipeline_memory(char **args, t_shell *shell,
	t_pipe_cleaner *cleanup)
{
	int	i;

	if (args)
		free_args(args);
	if (shell)
	{
		free_split(&shell->split_input);
		free_env(shell);
		free(shell->current_input);
	}
	if (cleanup)
	{
		if (cleanup->commands)
			free(cleanup->commands);
		if (cleanup->pipes)
		{
			i = -1;
			while (++i < cleanup->cmd_count - 1)
				if (cleanup->pipes[i])
					free(cleanup->pipes[i]);
			free(cleanup->pipes);
		}
		if (cleanup->pids)
			free(cleanup->pids);
	}
}

void	free_everything_in_child(char *delim, t_heredoc_sub heresub,
	t_pipe_ctx *pipeline_ctx, char *temp_filename)
{
	int	i;

	i = -1;
	free(delim);
	free_split(&(pipeline_ctx->shell->split_input));
	free_env(pipeline_ctx->shell);
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
	exit(EXIT_SUCCESS);
}

void	free_heredoc(t_shell *shell, t_heredoc_params *params)
{
	if (params->args)
		free_args(params->args);
	if (params->clean_args)
		free_args(params->clean_args);
	if (params->delimiter)
		free(params->delimiter);
	if (shell)
	{
		free_env(shell);
		free_split(&shell->split_input);
		free(shell->current_input);
	}
}
