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
 * Deep-copies a t_split structure to a char** array using ft_strdup continuously. char** arrays are suitable for execve().
 * If initial array allocation fails, returns NULL immediately. Also returns NULL-terminated array as required by POSIX exec* functions.
 * This function is called before every execve() to prepare arguments.
 * The returned array is completely independent of the input split structure.
 * Caller side is responsible for freeing the returned array with free_args()!
 */
char	**split_to_args(t_split split)
{
	char	**args;
	int		i;
	int		j;
	int		k;

	args = malloc(sizeof(char *) * (split.size + 1));
	if (!args)
		return (NULL);
	i = 0;
	j = 0;
	while (i < split.size)
	{
		if (split.start[i] && split.start[i][0] != '\0')
		{
			k = 0;
			while (split.start[i][k] && (split.start[i][k] == ' ' || 
				split.start[i][k] == '\t'))
				k++;
			if (split.start[i][k] == '\0')
			{
				i++;
				continue;
			}
			args[j] = ft_strdup(split.start[i]);
			if (!args[j])
			{
				free_args(args);
				return (NULL);
			}
			j++;
		}
		i++;
	}
	args[j] = NULL;
	return (args);
}

char	**filter_empty_args(char **args)
{
	char	**filtered;
	int		count;
	int		i;
	int		j;

	if (!args)
		return (NULL);
	count = 0;
	i = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
			count++;
		i++;
	}
	filtered = malloc(sizeof(char *) * (count + 1));
	if (!filtered)
		return (NULL);
	i = 0;
	j = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
		{
			filtered[j] = ft_strdup(args[i]);
			if (!filtered[j])
			{
				free_args(filtered);
				return (NULL);
			}
			j++;
		}
		i++;
	}
	filtered[j] = NULL;
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

