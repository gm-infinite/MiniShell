/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_arg_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 23:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 23:46:49 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

extern char	*wildcard_handle(char *wildcard, t_shell *shell);

int	is_empty_or_whitespace(char *str)
{
	int	k;

	k = 0;
	while (str[k] && (str[k] == ' ' || str[k] == '\t'))
		k++;
	return (str[k] == '\0');
}

int	count_non_empty_args(char **args)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
			count++;
		i++;
	}
	return (count);
}

int	copy_non_empty_args(char **args, char **filtered)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
		{
			filtered[j] = ft_strdup(args[i]);
			if (!filtered[j])
				return (-1);
			j++;
		}
		i++;
	}
	return (j);
}

void	process_and_check_args(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	if (!args[0])
	{
		free_args(args);
		exit(0);
	}
	i = -1;
	while (args[++i])
	{
		expanded = expand_variables_quoted(args[i], shell);
		if (expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
	}
	process_args_quotes(args, shell);
	if (!args[0])
		handle_empty_pipe_args(args);
}

void	handle_empty_pipe_args(char **args)
{
	if (args[0] && args[0][0] == '\0')
		write(STDERR_FILENO, ": command not found\n", 20);
	free_args(args);
	if (args[0] && args[0][0] == '\0')
		exit(127);
	else
		exit(0);
}

// Apply wildcard expansion to arguments after variable expansion
// This ensures wildcards in variables are expanded at the right time
void	expand_wildcards_in_args(char **args, t_shell *shell)
{
	t_split		input;
	char		*expanded;
	int			i;
	
	if (!args)
		return ;
	
	// Create a temporary split from the args
	i = 0;
	while (args[i])
		i++;
	input.size = i;
	input.start = args;
	
	// Convert to string, apply wildcard expansion, then split back
	expanded = revert_split_str(input);
	if (expanded)
	{
		char *wildcard_expanded = wildcard_input_modify(expanded, shell);
		if (wildcard_expanded && wildcard_expanded != expanded)
		{
			// Parse the expanded result back into args
			t_split new_split = create_split_str(wildcard_expanded);
			if (new_split.size > 0 && new_split.size <= input.size)
			{
				// Copy expanded args back
				i = 0;
				while (i < new_split.size && args[i])
				{
					free(args[i]);
					args[i] = ft_strdup(new_split.start[i]);
					i++;
				}
				// Clear remaining args if any
				while (args[i])
				{
					free(args[i]);
					args[i] = NULL;
					i++;
				}
			}
			free_split(&new_split);
			free(wildcard_expanded);
		}
		else if (wildcard_expanded == expanded)
		{
			// No change needed
		}
		free(expanded);
	}
}
