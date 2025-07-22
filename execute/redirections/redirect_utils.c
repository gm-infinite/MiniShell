/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
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

static int	handle_redirection_type(int redirect_type, char *processed_filename,
		t_redir_fds *fds)
{
	if (redirect_type == 1)
	{
		if (ft_strncmp(processed_filename, "/tmp/.minishell_heredoc_", 24) == 0)
			return (handle_heredoc_file_cleanup(processed_filename, fds));
		else
			return (handle_here_document(processed_filename, fds));
	}
	else if (redirect_type == 3)
		return (handle_input_redirection(processed_filename, fds));
	else if (redirect_type == 2 || redirect_type == 4 || redirect_type == 5)
		return (handle_output_redirect(processed_filename, fds, redirect_type));
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

int	process_single_redirection(char **args, int i, t_redir_fds *fds,
		t_shell *shell)
{
	int		redirect_type;
	char	*processed_filename;

	redirect_type = is_redirection(args[i]);
	if (!args[i + 1])
	{
		write(STDERR_FILENO, "syntax error: missing filename\n", 32);
		return (-1);
	}
	processed_filename = get_processed_filename(args, i, redirect_type, shell);
	if (!processed_filename)
		return (-1);
	if (handle_redirection_type(redirect_type, processed_filename, fds) == -1)
	{
		perror(processed_filename);
		if (processed_filename != args[i + 1])
			free(processed_filename);
		return (-1);
	}
	if (processed_filename != args[i + 1])
		free(processed_filename);
	return (0);
}

char	**build_clean_args(char **args, int clean_count)
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
