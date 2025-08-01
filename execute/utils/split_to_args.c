/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_to_args.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:15:44 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 13:30:51 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static void	cleanup_args_on_error(char **args, int up_to_index)
{
	while (--up_to_index >= 0)
		free(args[up_to_index]);
	free(args);
}

static int	copy_split_strings(char **args, t_split split)
{
	int	i;

	i = 0;
	while (i < split.size)
	{
		if (split.start[i])
		{
			args[i] = ft_strdup(split.start[i]);
			if (!args[i])
			{
				cleanup_args_on_error(args, i);
				return (-1);
			}
		}
		else
			args[i] = NULL;
		i++;
	}
	args[i] = NULL;
	return (0);
}

static char	**allocate_args_array(int size)
{
	char	**args;

	args = malloc(sizeof(char *) * (size + 1));
	return (args);
}

char	**split_to_args(t_split split)
{
	char	**args;

	args = allocate_args_array(split.size);
	if (!args)
		return (NULL);
	if (copy_split_strings(args, split) == -1)
		return (NULL);
	return (args);
}
