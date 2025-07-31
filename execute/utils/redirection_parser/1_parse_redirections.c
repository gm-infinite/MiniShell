/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1_parse_redirections.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:23:57 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:28:34 by emgenc           ###   ########.fr       */
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
