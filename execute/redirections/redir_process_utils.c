/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_process_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 21:51:44 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

char	*process_filename(char *filename, t_shell *shell)
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

int	delimiter_was_quoted(char *filename)
{
	int	i;

	if (!filename)
		return (0);
	i = 0;
	while (filename[i])
	{
		if (filename[i] == '\'' || filename[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

char	*process_heredoc_delimiter(char *filename)
{
	char	*result;

	result = remove_quotes_for_redirection(filename);
	if (!result)
	{
		result = ft_strdup(filename);
		if (!result)
			return (NULL);
	}
	return (result);
}

int	clean_heredoc(char *processed_filename, t_redir_fds *fds)
{
	*fds->input_fd = open(processed_filename, O_RDONLY);
	if (*fds->input_fd == -1)
		return (-1);
	unlink(processed_filename);
	return (0);
}

int	handle_input_redirection(char *processed_filename, t_redir_fds *fds)
{
	if (not_dir(processed_filename))
	{
		write_notdir(processed_filename);
		errno = ENOTDIR;
		return (-1);
	}
	*fds->input_fd = open(processed_filename, O_RDONLY);
	if (*fds->input_fd == -1)
		return (-1);
	return (0);
}
