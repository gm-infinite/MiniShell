/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_type_handler.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:05:18 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 04:46:51 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

static int	handle_output_redirect(char *processed_filename, t_redir_fds *fds,
		int redirect_type)
{
	if (not_dir(processed_filename))
	{
		write_notdir(processed_filename);
		errno = ENOTDIR;
		return (-1);
	}
	if (redirect_type == 2)
		*fds->output_fd = open(processed_filename,
				O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (redirect_type == 4)
		*fds->output_fd = open(processed_filename,
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redirect_type == 5)
		*fds->stderr_fd = open(processed_filename,
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if ((redirect_type == 2 || redirect_type == 4) && *fds->output_fd == -1)
		return (-1);
	if (redirect_type == 5 && *fds->stderr_fd == -1)
		return (-1);
	return (0);
}

static int	handle_input_redirection(char *processed_filename, t_redir_fds *fds)
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

int	handle_redirection_type(t_redirect_info *info, t_redir_fds *fds,
		t_shell *shell, t_heredoc_params *params)
{
	if (info->redirect_type == 1)
	{
		if (ft_strncmp(info->processed_filename, "/tmp/.minishell_heredoc_", 24)
			== 0)
			return (clean_heredoc(info->processed_filename, fds));
		else
			return (handle_here_document(info->processed_filename, fds, shell,
					params));
	}
	else if (info->redirect_type == 3)
		return (handle_input_redirection(info->processed_filename, fds));
	else if (info->redirect_type == 2 || info->redirect_type == 4
		|| info->redirect_type == 5)
		return (handle_output_redirect(info->processed_filename, fds,
				info->redirect_type));
	return (0);
}
