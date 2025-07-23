/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_args_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:40:47 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

char	**allocate_args_array(int size)
{
	char	**args;

	args = malloc(sizeof(char *) * (size + 1));
	return (args);
}

void	cleanup_args_on_error(char **args, int up_to_index)
{
	while (--up_to_index >= 0)
		free(args[up_to_index]);
	free(args);
}

int	copy_split_strings(char **args, t_split split)
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
