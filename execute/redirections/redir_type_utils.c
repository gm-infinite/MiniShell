/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_type_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 23:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 20:47:29 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	handle_here_document(char *processed_delimiter, t_redir_fds *fds,
		t_shell *shell, t_heredoc_params *params)
{
	int	here_doc_pipe[2];
	int	should_expand;
	int	ret;

	should_expand = !delimiter_was_quoted(params->delimiter);
	params->delimiter = processed_delimiter;
	ret = handle_here_doc(here_doc_pipe, shell, should_expand, params);
	if (ret != 0)
		return (-1);
	*fds->input_fd = here_doc_pipe[0];
	return (0);
}

static int	open_append_file(char *filename)
{
	return (open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644));
}

static int	open_trunc_file(char *filename)
{
	return (open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644));
}

int	handle_output_redirect(char *processed_filename, t_redir_fds *fds,
		int redirect_type)
{
	if (has_not_directory_error(processed_filename))
	{
		write_not_directory_error(processed_filename);
		errno = ENOTDIR;
		return (-1);
	}
	if (redirect_type == 2)
		*fds->output_fd = open_append_file(processed_filename);
	else if (redirect_type == 4)
		*fds->output_fd = open_trunc_file(processed_filename);
	else if (redirect_type == 5)
		*fds->stderr_fd = open_trunc_file(processed_filename);
	if ((redirect_type == 2 || redirect_type == 4) && *fds->output_fd == -1)
		return (-1);
	if (redirect_type == 5 && *fds->stderr_fd == -1)
		return (-1);
	return (0);
}
