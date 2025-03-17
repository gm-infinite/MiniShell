/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 12:45:10 by emgenc            #+#    #+#             */
/*   Updated: 2025/03/17 11:02:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../t_split_utils/t_split.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>

int	unset(char *argv[], char **envp)
{
	int		i;
	int		j;
	size_t	var_len;
	int		iterator;

	// CONTROLS //

	// unset: not enough arguments
	if (!argv[1])
		return (printf("unset: not enough arguments\n"), 1);
	// unset: not a valid identifier
	iterator = -1;
	if (ft_isdigit(argv[1]) || !ft_isalpha(argv[1]))
		return (printf("unset: not a valid identifier\n"), 1);
	while (argv[++iterator])
	{
		var_len = ft_strlen(argv[iterator]);
		i = -1;
		while (envp[++i])
		{
			if (ft_strncmp(envp[i], argv[iterator], var_len) == 0
				&& envp[i][var_len] == '=')
			{// Rearrange the envp array
				free(envp[i]);
				j = i;
				while (envp[j])
				{
					envp[j] = envp[j + 1];
					j++;
				}
				break ;
			}
		}
	}
}
