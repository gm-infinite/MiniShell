/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_expander_loop.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:17:57 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 11:43:36 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

char	*wildcard_expand(char *reconstructed)
{
	char	*wildcard_expanded;

	wildcard_expanded = wildcard_input_modify(reconstructed);
	if (wildcard_expanded && wildcard_expanded != reconstructed)
	{
		free(reconstructed);
		return (wildcard_expanded);
	}
	return (reconstructed);
}

char	*reconstructed_args(char **args)
{
	char	*reconstructed;
	char	*temp;
	int		i;

	reconstructed = NULL;
	i = -1;
	while (args[++i])
	{
		if (reconstructed == NULL)
			reconstructed = ft_strdup(args[i]);
		else
		{
			temp = ft_strjoin(reconstructed, " ");
			free(reconstructed);
			reconstructed = temp;
			temp = ft_strjoin(reconstructed, args[i]);
			free(reconstructed);
			reconstructed = temp;
		}
	}
	return (reconstructed);
}

char	**arg_expander_loop(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	i = -1;
	while (args[++i])
	{
		expanded = expandvar_quoted(args[i], shell);
		if (expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
	}
	return (args);
}
