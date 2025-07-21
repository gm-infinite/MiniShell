/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:40:47 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

/**
 * Deep-copies a t_split structure to a char** array using ft_strdup.
 * char** arrays are suitable for execve().
 * Returns NULL-terminated array as required by POSIX exec* functions.
 * This function is called before every execve() to prepare arguments.
 * The returned array is completely independent of the input split structure.
 * Caller side is responsible for freeing the returned array with free_args()!
 */
char	**split_to_args(t_split split)
{
	char	**args;
	int		i;
	int		j;

	args = malloc(sizeof(char *) * (split.size + 1));
	if (!args)
		return (NULL);
	i = -1;
	j = 0;
	while (++i < split.size)
	{
		if (split.start[i] && split.start[i][0] != '\0'
			&& !is_empty_or_whitespace(split.start[i]))
		{
			args[j] = ft_strdup(split.start[i]);
			if (!args[j])
			{
				free_args(args);
				return (NULL);
			}
			j++;
		}
	}
	args[j] = NULL;
	return (args);
}

char	**filter_empty_args(char **args)
{
	char	**filtered;
	int		count;
	int		result;

	if (!args)
		return (NULL);
	count = count_non_empty_args(args);
	filtered = malloc(sizeof(char *) * (count + 1));
	if (!filtered)
		return (NULL);
	result = copy_non_empty_args(args, filtered);
	if (result == -1)
	{
		free_args(filtered);
		return (NULL);
	}
	filtered[result] = NULL;
	return (filtered);
}

void	free_args(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}
