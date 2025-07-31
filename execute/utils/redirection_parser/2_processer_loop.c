/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_processer_loop.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:56:59 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 19:26:07 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

static char	*process_filename(char *filename, t_shell *shell)
{
	char	*expanded_with_quotes;
	char	*final_result;

	expanded_with_quotes = process_q_expand(filename, shell);
	if (!expanded_with_quotes)
		return (NULL);
	final_result = remove_quotes_for_redirection(expanded_with_quotes);
	if (!final_result)
		final_result = ft_strdup(expanded_with_quotes);
	free(expanded_with_quotes);
	return (final_result);
}

static char	*get_processed_filename(char **args, int i, int redirect_type,
		t_shell *shell)
{
	if (redirect_type == 1)
		return (process_heredoc_delimiter(args[i + 1]));
	else
		return (process_filename(args[i + 1], shell));
}

t_redirect_info	get_redirect_info(char **args, int i, t_shell *shell)
{
	t_redirect_info	info;

	info.redirect_type = is_redirection(args[i]);
	if (!args[i + 1])
	{
		info.redirect_type = -1;
		info.processed_filename = NULL;
		return (info);
	}
	info.processed_filename = get_processed_filename(args, i,
			info.redirect_type, shell);
	return (info);
}

int	after_handle(t_redirect_info *redirect_info,
	t_heredoc_params *params, int i)
{
	if (redirect_info->redirect_type != 1 && errno != ENOTDIR)
		perror(redirect_info->processed_filename);
	if (redirect_info->processed_filename != params->args[i + 1])
		free(redirect_info->processed_filename);
	return (-1);
}

int	process_redirections_loop(char **args, t_redir_fds *fds,
	t_shell *shell, char **clean_args)
{
	int					i;
	t_heredoc_params	params;

	i = 0;
	while (args[i])
	{
		if (is_redirection(args[i]))
		{
			params.args = args;
			params.clean_args = clean_args;
			params.delimiter = NULL;
			if (process_single_redir(i, fds, shell, &params) == -1)
				return (-1);
			i += 2;
		}
		else
			i++;
	}
	return (0);
}
