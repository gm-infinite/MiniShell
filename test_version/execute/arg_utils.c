/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	has_redirections(t_split split)
{
	int	i;

	i = 0;
	while (i < split.size)
	{
		if (is_redirection(split.start[i]))
			return (1);
		i++;
	}
	return (0);
}

static void	move_arg(char **args, int read_pos, int write_pos)
{
	if (read_pos != write_pos)
	{
		args[write_pos] = args[read_pos];
		args[read_pos] = NULL;
	}
}

static void	free_empty_arg(char **args, int read_pos)
{
	free(args[read_pos]);
	args[read_pos] = NULL;
}

void	compact_args(char **args)
{
	int	read_pos;
	int	write_pos;

	if (!args)
		return ;
	read_pos = 0;
	write_pos = 0;
	while (args[read_pos])
	{
		if (args[read_pos][0] != '\0')
		{
			move_arg(args, read_pos, write_pos);
			write_pos++;
		}
		else
			free_empty_arg(args, read_pos);
		read_pos++;
	}
	args[write_pos] = NULL;
}
