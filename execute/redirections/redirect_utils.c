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
			int *input_fd, int *output_fd, int *stderr_fd)
{
	int	here_doc_pipe[2];

	if (redirect_type == 1)
	{
		if (handle_here_doc(processed_filename, here_doc_pipe) == 0)
			*input_fd = here_doc_pipe[0];
	}
	else if (redirect_type == 2)
	{
		*output_fd = open(processed_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (*output_fd == -1)
			return (-1);
	}
	else if (redirect_type == 3)
	{
		*input_fd = open(processed_filename, O_RDONLY);
		if (*input_fd == -1)
			return (-1);
	}
	else if (redirect_type == 4)
	{
		*output_fd = open(processed_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (*output_fd == -1)
			return (-1);
	}
	else if (redirect_type == 5)
	{
		*stderr_fd = open(processed_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (*stderr_fd == -1)
			return (-1);
	}
	return (0);
}

static char	*process_filename(char *filename, t_shell *shell)
{
	char	*quoted_removed;
	char	*processed_filename;

	quoted_removed = remove_quotes_for_redirection(filename);
	if (!quoted_removed)
	{
		quoted_removed = ft_strdup(filename);
		if (!quoted_removed)
			return (NULL);
	}
	processed_filename = expand_variables(quoted_removed, shell);
	free(quoted_removed);
	return (processed_filename);
}

int	process_single_redirection(char **args, int i, int *input_fd,
		int *output_fd, int *stderr_fd, t_shell *shell)
{
	int		redirect_type;
	char	*processed_filename;

	redirect_type = is_redirection(args[i]);
	if (!args[i + 1])
	{
		write(STDERR_FILENO, "syntax error: missing filename\n", 32);
		return (-1);
	}
	processed_filename = process_filename(args[i + 1], shell);
	if (!processed_filename)
		return (-1);
	if (handle_redirection_type(redirect_type, processed_filename,
			input_fd, output_fd, stderr_fd) == -1)
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
