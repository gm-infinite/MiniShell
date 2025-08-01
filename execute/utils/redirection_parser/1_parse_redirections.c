/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1_parse_redirections.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:23:57 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 19:02:29 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

static int	count_clean_args(char **args)
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

static char	**build_clean_args(char **args, int clean_count)
{
	char	**clean_args;
	int		i;
	int		j;
	int		redirect_type;

	clean_args = malloc(sizeof(char *) * (clean_count + 1));
	if (!clean_args)
		return (NULL);
	i = 0;
	j = 0;
	while (args[i])
	{
		redirect_type = is_redirection(args[i]);
		if (redirect_type)
			i += 2;
		else
		{
			clean_args[j] = ft_strdup(args[i]);
			j++;
			i++;
		}
	}
	clean_args[j] = NULL;
	return (clean_args);
}

static void	redirection_failed(t_redir_fds *fds)
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

char	**parse_redirs(t_split split, t_redir_fds *fds, t_shell *shell)
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
		redirection_failed(fds);
		return (NULL);
	}
	free_args(args);
	return (clean_args);
}

int	process_single_redir(int i, t_redir_fds *fds,
	t_shell *shell, t_heredoc_params *params)
{
	t_redirect_info	redirect_info;
	int				rc;

	redirect_info = get_redirect_info(params->args, i, shell);
	if (redirect_info.redirect_type == 1 && fds->preprocessed_heredoc)
	{
		if (redirect_info.processed_filename != params->args[i + 1])
			free(redirect_info.processed_filename);
		return (0);
	}
	if (redirect_info.redirect_type == -1)
	{
		write(STDERR_FILENO, "syntax error: missing filename\n", 32);
		return (-1);
	}
	if (!redirect_info.processed_filename)
		return (-1);
	params->delimiter = params->args[i + 1];
	rc = handle_redirection_type(&redirect_info, fds, shell, params);
	if (rc == -1)
		return (after_handle(&redirect_info, params, i));
	if (redirect_info.processed_filename != params->args[i + 1])
		free(redirect_info.processed_filename);
	return (0);
}
