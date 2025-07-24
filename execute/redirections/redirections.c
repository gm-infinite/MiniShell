/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 14:15:09 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 11:49:25 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

void	redirection_fail_procedure(t_redir_fds *fds)
{
	if (*fds->input_fd != STDIN_FILENO && *fds->input_fd != -1)
	{
		close(*fds->input_fd);
		*fds->input_fd = STDIN_FILENO;
	}
	if (*fds->output_fd != STDOUT_FILENO && *fds->output_fd != -1)
	{
		close(*fds->output_fd);
		*fds->output_fd = STDOUT_FILENO;
	}
	if (*fds->stderr_fd != STDERR_FILENO && *fds->stderr_fd != -1)
	{
		close(*fds->stderr_fd);
		*fds->stderr_fd = STDERR_FILENO;
	}
}

static int	process_redirections_loop(char **args, t_redir_fds *fds,
		t_shell *shell, char **clean_args)
{
	int	i;

	i = 0;
	while (args[i])
	{
		if (is_redirection(args[i]))
		{
			if (process_single_redirection(args, i, fds, shell, clean_args) == -1)
				return (-1);
			i += 2;
		}
		else
			i++;
	}
	return (0);
}

char	**parse_redirections(t_split split, t_redir_fds *fds, t_shell *shell)
{
	char		**args;
	char		**clean_args;
	int			clean_count;

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
	if (process_redirections_loop(args, fds, shell, clean_args) == -1)
	{
		free_args(args);
		free_args(clean_args);
		redirection_fail_procedure(fds);
		return (NULL);
	}
	free_args(args);
	return (clean_args);
}

static int	handle_redirected_execution(t_redir_exec_context *ctx)
{
	int	result;

	result = handle_empty_args(ctx->args, ctx->input_fd, ctx->output_fd,
			ctx->stderr_fd);
	if (result != -1)
		return (result);
	process_variable_expansion(ctx->args, ctx->shell);
	result = check_empty_command_after_expansion(ctx->args, ctx->input_fd,
			ctx->output_fd, ctx->stderr_fd);
	if (result != -1)
		return (result);
	if (is_builtin(ctx->args[0]))
		result = execute_builtin_with_redirect(ctx);
	else
		result = execute_external_with_redirect(ctx);
	free_args(ctx->args);
	return (result);
}

int	execute_with_redirections(t_split split, t_shell *shell)
{
	t_redir_exec_context	ctx;
	t_redir_fds				fds;

	ctx.input_fd = STDIN_FILENO;
	ctx.output_fd = STDOUT_FILENO;
	ctx.stderr_fd = STDERR_FILENO;
	ctx.shell = shell;
	fds.input_fd = &ctx.input_fd;
	fds.output_fd = &ctx.output_fd;
	fds.stderr_fd = &ctx.stderr_fd;
	ctx.args = parse_redirections(split, &fds, shell);
	if (!ctx.args)
		return (1);
	return (handle_redirected_execution(&ctx));
}
