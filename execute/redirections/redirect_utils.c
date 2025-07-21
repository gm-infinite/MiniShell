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
		return (handle_here_document(processed_filename, fds));
	else if (redirect_type == 3)
	{
		*fds->input_fd = open(processed_filename, O_RDONLY);
		if (*fds->input_fd == -1)
			return (-1);
	}
	else if (redirect_type == 2 || redirect_type == 4 || redirect_type == 5)
		return (handle_output_redirect(processed_filename, fds, redirect_type));
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

static char	*process_heredoc_delimiter(char *filename, t_shell *shell)
{
	char	*result;
	int		has_double_quotes;
	int		i;

	// Check if the original filename has double quotes
	has_double_quotes = 0;
	i = 0;
	while (filename[i])
	{
		if (filename[i] == '"')
		{
			has_double_quotes = 1;
			break;
		}
		i++;
	}

	// Remove quotes first
	result = remove_quotes_for_redirection(filename);
	if (!result)
	{
		result = ft_strdup(filename);
		if (!result)
			return (NULL);
	}

	// Only expand variables if original had double quotes
	if (has_double_quotes)
	{
		char *expanded = expand_variables(result, shell);
		free(result);
		return (expanded);
	}

	// For unquoted or single-quoted, return as-is (no expansion)
	return (result);
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
	
	// Use special heredoc delimiter processing for heredoc (redirect_type == 1)
	if (redirect_type == 1)
		processed_filename = process_heredoc_delimiter(args[i + 1], shell);
	else
		processed_filename = process_filename(args[i + 1], shell);
	
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
