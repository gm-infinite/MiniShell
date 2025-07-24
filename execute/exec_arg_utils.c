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
#include "../parser/wildcard_handle/wildcard_handler.h"

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
