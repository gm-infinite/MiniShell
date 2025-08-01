/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:27:11 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 15:32:29 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../parser/wildcard_handle/wildcard_handler.h"

static int	has_redirs(t_split split)
{
	int	i;

	i = 0;
	while (i < split.size)
	{
		if (is_redirection(split.start[i]))
			return (1);
		i++;
	}
	return (0);
}

static int	dispatch_execution(t_split split, t_shell *shell,
		int has_pipes, int has_redir)
{
	if (has_pipes > 0 && has_redir)
		return (execute_pipe_redir(split, shell));
	else if (has_pipes > 0)
		return (execute_pipeline(split, shell));
	else if (has_redir)
		return (execute_with_redirections(split, shell));
	else
		return (execute_args_array(split, shell));
}

int	execute_command(t_split split, t_shell *shell)
{
	int	exit_status;
	int	has_pipes;
	int	has_redir;
	int	syntax_error;

	if (split.size == 0)
		return (0);
	syntax_error = validate_redirection_syntax(split);
	if (syntax_error != 0)
	{
		shell->past_exit_status = syntax_error;
		return (syntax_error);
	}
	has_pipes = count_pipes(split);
	has_redir = has_redirs(split);
	exit_status = dispatch_execution(split, shell, has_pipes, has_redir);
	shell->past_exit_status = exit_status;
	return (exit_status);
}

int	is_redirection(char *token)
{
	if (!token)
		return (0);
	if (ft_strncmp(token, "<<", 3) == 0 && ft_strlen(token) == 2)
		return (1);
	if (ft_strncmp(token, ">>", 3) == 0 && ft_strlen(token) == 2)
		return (2);
	if (ft_strncmp(token, "<", 2) == 0 && ft_strlen(token) == 1)
		return (3);
	if (ft_strncmp(token, ">", 2) == 0 && ft_strlen(token) == 1)
		return (4);
	return (0);
}
