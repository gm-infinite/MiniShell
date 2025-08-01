/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs_only.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:20:46 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 16:18:26 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static int	check_empty_command_after_expansion(char **args, int input_fd,
										int output_fd, int stderr_fd)
{
	if (!args[0])
	{
		if (input_fd != STDIN_FILENO)
			close(input_fd);
		if (output_fd != STDOUT_FILENO)
			close(output_fd);
		if (stderr_fd != STDERR_FILENO)
			close(stderr_fd);
		free_args(args);
		return (0);
	}
	if (args[0][0] == '\0')
	{
		write(STDERR_FILENO, ": command not found\n", 20);
		free_args(args);
		return (127);
	}
	return (-1);
}

static void	process_variable_expansion(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	i = 0;
	while (args[i])
	{
		expanded = expandvar(args[i], shell);
		if (expanded && expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}
	process_args_quotes(args, shell);
}

static int	handle_redirected_execution(t_redir_exec_ctx *ctx)
{
	int	result;

	if (!(ctx->args[0]))
	{
		if (ctx->input_fd != STDIN_FILENO)
			close(ctx->input_fd);
		if (ctx->output_fd != STDOUT_FILENO)
			close(ctx->output_fd);
		if (ctx->stderr_fd != STDERR_FILENO)
			close(ctx->stderr_fd);
		return (0);
	}
	process_variable_expansion(ctx->args, ctx->shell);
	result = check_empty_command_after_expansion(ctx->args, ctx->input_fd,
			ctx->output_fd, ctx->stderr_fd);
	if (result != -1)
		return (result);
	if (is_builtin(ctx->args[0]))
		result = execute_builtin_with_redirect(ctx);
	else
		result = execute_external_with_redirect(ctx);
	return (result);
}

int	execute_with_redirections(t_split split, t_shell *shell)
{
	t_redir_exec_ctx	ctx;
	t_redir_fds			fds;
	int					ret;

	ctx.input_fd = STDIN_FILENO;
	ctx.output_fd = STDOUT_FILENO;
	ctx.stderr_fd = STDERR_FILENO;
	ctx.shell = shell;
	fds.input_fd = &ctx.input_fd;
	fds.output_fd = &ctx.output_fd;
	fds.stderr_fd = &ctx.stderr_fd;
	fds.preprocessed_heredoc = 0;
	ctx.args = parse_redirs(split, &fds, shell);
	if (!ctx.args)
		return (1);
	ret = handle_redirected_execution(&ctx);
	free_args(ctx.args);
	return (ret);
}
