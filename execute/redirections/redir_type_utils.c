/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_type_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 23:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 18:25:32 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int handle_here_document(char *processed_delimiter,
                         t_redir_fds *fds,
                         t_shell   *shell,
                         char      *original_delimiter)
{
    int here_doc_pipe[2];
    int should_expand;
    int ret;

    should_expand = !delimiter_was_quoted(original_delimiter);
    ret = handle_here_doc(processed_delimiter,
                          here_doc_pipe,
                          shell,
                          should_expand);
    if (ret != 0)              /* on fork/read error or user ^C */
        return (-1);

    /* success: hook up the read‑end as stdin for the command */
    *fds->input_fd = here_doc_pipe[0];
    return (0);
}

int	handle_output_redirect(char *processed_filename, t_redir_fds *fds,
		int redirect_type)
{
	if (redirect_type == 2)
	{
		*fds->output_fd = open(processed_filename, O_WRONLY | O_CREAT
				| O_APPEND, 0644);
		if (*fds->output_fd == -1)
			return (-1);
	}
	else if (redirect_type == 4)
	{
		*fds->output_fd = open(processed_filename, O_WRONLY | O_CREAT
				| O_TRUNC, 0644);
		if (*fds->output_fd == -1)
			return (-1);
	}
	else if (redirect_type == 5)
	{
		*fds->stderr_fd = open(processed_filename, O_WRONLY | O_CREAT
				| O_TRUNC, 0644);
		if (*fds->stderr_fd == -1)
			return (-1);
	}
	return (0);
}
