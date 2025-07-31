/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_processer_loop.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:56:59 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 16:04:39 by emgenc           ###   ########.fr       */
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

static t_redirect_info	get_redirect_info(char **args, int i, t_shell *shell)
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

int process_single_redir(int i, t_redir_fds *fds, t_shell *shell,
                         t_heredoc_params *params)
{
    t_redirect_info redirect_info;
    int             rc;

    redirect_info = get_redirect_info(params->args, i, shell);

    // If this is a here-doc AND stdin was pre-set by preprocess_heredocs,
    // skip re-processing it in the child.
    if (redirect_info.redirect_type == 1 && fds->preprocessed_heredoc)
	{
		if (redirect_info.processed_filename != params->args[i+1])
        	free(redirect_info.processed_filename);
		return 0;
	}

    if (redirect_info.redirect_type == -1)
    {
        write(STDERR_FILENO, "syntax error: missing filename\n", 32);
        return -1;
    }
    if (!redirect_info.processed_filename)
        return -1;

    params->delimiter = params->args[i + 1];
    rc = handle_redirection_type(&redirect_info, fds, shell, params);
    if (rc == -1)
    {
        if (redirect_info.redirect_type != 1 && errno != ENOTDIR)
            perror(redirect_info.processed_filename);
        if (redirect_info.processed_filename != params->args[i + 1])
            free(redirect_info.processed_filename);
        return -1;
    }
    if (redirect_info.processed_filename != params->args[i + 1])
        free(redirect_info.processed_filename);
    return 0;
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
