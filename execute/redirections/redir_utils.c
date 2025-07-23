/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:52:11 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 18:25:24 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	is_redirection(char *token)
{
	if (!token)
		return (0);
	if (ft_strncmp(token, "<<", 3) == 0 && ft_strlen(token) == 2)
		return (1);
	if (ft_strncmp(token, ">>", 3) == 0 && ft_strlen(token) == 2)
		return (2);
	if (ft_strncmp(token, "<", 2) == 0 && ft_strlen(token) == 1)
		return (3);
	if (ft_strncmp(token, ">", 2) == 0 && ft_strlen(token) == 1)
		return (4);
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
