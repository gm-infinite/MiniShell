/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_with_redirs.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:33:52 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 13:55:32 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int preprocess_heredocs(t_pipe_ctx *ctx)
{
    ctx->heredoc_fds = malloc(sizeof(int) * ctx->cmd_count);
    if (!ctx->heredoc_fds) return 0;

    for (int i = 0; i < ctx->cmd_count; ++i) {
        char **args = ctx->commands[i].start;
        int fd = -1;
        for (int j = 0; args[j]; j++) {
            if (is_redirection(args[j]) == 1) {
                // args[j+1] is the delimiter
                char *delim = process_heredoc_delimiter(args[j+1]);
                int should_expand = !delimiter_was_quoted(args[j+1]);
                // call your working pipe-based handler directly
                if (handle_here_doc(&fd, ctx->shell, should_expand,
                                    &(t_heredoc_params){delim,args,NULL}) != 0) {
                    free(delim);
                    return 0; // error
                }
                free(delim);
            }
        }
        ctx->heredoc_fds[i] = fd;
    }
    return 1;
}

static int	wait_for_others(pid_t *pids, int cmd_count)
{
	int	status;
	int	exit_status;
	int	i;

	exit_status = 0;
	i = -1;
	while (++i < cmd_count)
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
	}
	return (exit_status);
}

static int	exec_pipe_child(t_pipe_ctx *pipeline_ctx)
{
	int	i;

	i = -1;
	while (++i < pipeline_ctx->cmd_count)
	{
		if (fork_pipe_child(pipeline_ctx, i) != 0)
			return (1);
	}
	close_all_pipes(pipeline_ctx->pipes, pipeline_ctx->cmd_count);
	return (wait_for_others(pipeline_ctx->pids, pipeline_ctx->cmd_count));
}

static int setup_and_execute_pipeline(t_split *commands, int cmd_count,
                                      t_shell *shell)
{
    int       **pipes;
    pid_t     *pids;
    t_pipe_ctx pipeline_ctx;

    if (!setup_pipe(&commands, &pipes, &pids, cmd_count))
        return (1);

    pipeline_ctx.commands     = commands;
    pipeline_ctx.pipes        = pipes;
    pipeline_ctx.pids         = pids;
    pipeline_ctx.cmd_count    = cmd_count;
    pipeline_ctx.shell        = shell;
    pipeline_ctx.heredoc_fds  = NULL;

    if (!preprocess_heredocs(&pipeline_ctx))
        return (1);

    /* Temporarily ignore SIGINT/SIGQUIT in the main shell */
    void (*old_sigint)(int)  = signal(SIGINT,  SIG_IGN);
    void (*old_sigquit)(int) = signal(SIGQUIT, SIG_IGN);

    cmd_count = exec_pipe_child(&pipeline_ctx);

    /* Restore the shell’s signal handlers */
    signal(SIGINT,  old_sigint);
    signal(SIGQUIT, old_sigquit);
    setup_signals();

    /* Close & free all heredoc fds */
    if (pipeline_ctx.heredoc_fds)
    {
        for (int i = 0; i < pipeline_ctx.cmd_count; ++i)
        {
            if (pipeline_ctx.heredoc_fds[i] >= 0)
                close(pipeline_ctx.heredoc_fds[i]);
        }
        free(pipeline_ctx.heredoc_fds);
        pipeline_ctx.heredoc_fds = NULL;
    }

    clean_pipe(commands, pipes, pids, pipeline_ctx.cmd_count);
    return (cmd_count);
}

int	execute_pipe_redir(t_split split, t_shell *shell)
{
	t_split	*commands;
	int		cmd_count;
	int		exit_status;

	commands = split_by_pipes(split, &cmd_count);
	if (!commands)
		return (1);
	if (cmd_count == 1)
	{
		exit_status = execute_with_redirections(commands[0], shell);
		free(commands);
		return (exit_status);
	}
	return (setup_and_execute_pipeline(commands, cmd_count, shell));
}
