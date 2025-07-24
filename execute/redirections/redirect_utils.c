/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 11:49:25 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	count_clean_args(char **args)
{
	int	i;
	int	j;
	int	redirect_type;

	i = 0;
	j = 0;
	while (args[i])
	{
		redirect_type = is_redirection(args[i]);
		if (redirect_type)
		{
			i += 2;
		}
		else
		{
			j++;
			i++;
		}
	}
	return (j);
}

static int	handle_redirection_type(t_redirect_info *info, t_redir_fds *fds,
		t_shell *shell, char *original_delimiter, char **args, char **clean_args)
{
	if (info->redirect_type == 1)
	{
		if (ft_strncmp(info->processed_filename, "/tmp/.minishell_heredoc_", 24)
			== 0)
			return (handle_heredoc_file_cleanup(info->processed_filename, fds));
		else
			return (handle_here_document(info->processed_filename, fds, shell,
					original_delimiter, args, clean_args));
	}
	else if (info->redirect_type == 3)
		return (handle_input_redirection(info->processed_filename, fds));
	else if (info->redirect_type == 2 || info->redirect_type == 4
		|| info->redirect_type == 5)
		return (handle_output_redirect(info->processed_filename, fds,
				info->redirect_type));
	return (0);
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

int	process_single_redir(char **args, int i, t_redir_fds *fds,
		t_shell *shell, char **clean_args)
{
	t_redirect_info	redirect_info;
	int				rc;

	redirect_info = get_redirect_info(args, i, shell);
	if (redirect_info.redirect_type == -1)
	{
		write(STDERR_FILENO, "syntax error: missing filename\n", 32);
		return (-1);
	}
	if (!redirect_info.processed_filename)
		return (-1);
	rc = handle_redirection_type(&redirect_info, fds, shell, args[i + 1], args, clean_args);
	if (rc == -1)
	{
		if (redirect_info.redirect_type != 1)
			perror(redirect_info.processed_filename);
		if (redirect_info.processed_filename != args[i + 1])
			free(redirect_info.processed_filename);
		return (-1);
	}
	if (redirect_info.processed_filename != args[i + 1])
		free(redirect_info.processed_filename);
	return (0);
}
