/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_wildcard_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 23:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 23:46:49 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../parser/wildcard_handle/wildcard_handler.h"

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

char	*build_temp_input(char **args)
{
	char	*temp_input;
	char	*temp;
	int		i;

	temp_input = NULL;
	i = 0;
	while (args[i])
	{
		if (temp_input == NULL)
			temp_input = ft_strdup(args[i]);
		else
		{
			temp = ft_strjoin(temp_input, " ");
			free(temp_input);
			temp_input = temp;
			temp = ft_strjoin(temp_input, args[i]);
			free(temp_input);
			temp_input = temp;
		}
		i++;
	}
	return (temp_input);
}

static void	update_args_from_split(char **args, t_split expanded_split)
{
	char	*unquoted;
	int		i;

	i = 0;
	while (i < expanded_split.size && args[i])
	{
		unquoted = remove_quotes_from_string(expanded_split.start[i]);
		free(args[i]);
		if (unquoted)
			args[i] = unquoted;
		else
			args[i] = ft_strdup(expanded_split.start[i]);
		i++;
	}
	while (args[i])
	{
		free(args[i]);
		args[i] = NULL;
		i++;
	}
}

void	expand_wildcards_in_args(char **args)
{
	char	*temp_input;
	char	*wildcard_expanded;
	t_split	expanded_split;

	if (!args || !args[0])
		return ;
	temp_input = build_temp_input(args);
	if (!validate_and_expand_input(temp_input, &wildcard_expanded))
		return ;
	if (!prepare_expanded_split(wildcard_expanded, &expanded_split))
		return ;
	update_args_from_split(args, expanded_split);
	free_split(&expanded_split);
}
