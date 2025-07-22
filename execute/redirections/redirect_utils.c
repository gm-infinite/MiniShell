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
	if (redirect_type == 1) // heredoc redirection
	{
		// Check if this is a pre-processed heredoc (temp file)
		if (ft_strncmp(processed_filename, "/tmp/.minishell_heredoc_", 24) == 0)
		{
			// This is a pre-processed heredoc, treat as input redirection
			*fds->input_fd = open(processed_filename, O_RDONLY);
			if (*fds->input_fd == -1)
				return (-1);
			// Clean up the temporary file
			unlink(processed_filename);
			return (0);
		}
		else
		{
			// Normal heredoc processing
			return (handle_here_document(processed_filename, fds));
		}
	}
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
static char	*process_quote_aware_expansion(char *str, t_shell *shell)
{
	char	*result;
	char	*temp;
	int		i;
	int		start;
	int		in_single_quotes;
	int		in_double_quotes;

	if (!str)
		return (NULL);

	result = ft_calloc(1, 1);
	if (!result)
		return (NULL);

	i = 0;
	in_single_quotes = 0;
	in_double_quotes = 0;

	while (str[i])
	{
		start = i;
		
		// Handle quote characters - preserve them in output
		if (str[i] == '\'' && !in_double_quotes)
		{
			// Add the quote to result
			char quote_str[2] = {'\'', '\0'};
			temp = ft_strjoin(result, quote_str);
			free(result);
			result = temp;
			
			in_single_quotes = !in_single_quotes;
			i++;
			continue;
		}
		else if (str[i] == '"' && !in_single_quotes)
		{
			// Add the quote to result
			char quote_str[2] = {'"', '\0'};
			temp = ft_strjoin(result, quote_str);
			free(result);
			result = temp;
			
			in_double_quotes = !in_double_quotes;
			i++;
			continue;
		}

		// Find the end of the current content segment
		while (str[i] && 
			   ((str[i] != '\'' || in_double_quotes) && 
				(str[i] != '"' || in_single_quotes)))
			i++;

		if (i > start)
		{
			// Extract segment
			char *segment = ft_substr(str, start, i - start);
			if (!segment)
			{
				free(result);
				return (NULL);
			}

			// Expand variables only if not in single quotes
			if (!in_single_quotes)
			{
				char *expanded = expand_variables(segment, shell);
				free(segment);
				segment = expanded;
			}

			// Append to result
			temp = ft_strjoin(result, segment ? segment : "");
			free(result);
			free(segment);
			result = temp;
		}
	}

	return (result);
}

static char	*process_filename(char *filename, t_shell *shell)
{
	char	*expanded_with_quotes;
	char	*final_result;

	// First: expand variables while respecting quote boundaries
	expanded_with_quotes = process_quote_aware_expansion(filename, shell);
	if (!expanded_with_quotes)
		return (NULL);

	// Second: remove quotes from the expanded result
	final_result = remove_quotes_for_redirection(expanded_with_quotes);
	if (!final_result)
	{
		final_result = ft_strdup(expanded_with_quotes);
	}
	
	free(expanded_with_quotes);
	return (final_result);
}

char	*process_heredoc_delimiter(char *filename, t_shell *shell)
{
	char	*result;

	(void)shell; // Unused parameter - heredoc delimiters are never expanded

	// Remove quotes but NEVER expand variables
	// Heredoc delimiters are always treated literally in bash
	result = remove_quotes_for_redirection(filename);
	if (!result)
	{
		result = ft_strdup(filename);
		if (!result)
			return (NULL);
	}

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
