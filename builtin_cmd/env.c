/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 12:45:02 by emgenc            #+#    #+#             */
/*   Updated: 2025/03/17 11:02:57 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../t_split_utils/t_split.h"

void	*env(char **envp)
{
	char	**traversal;

	traversal = envp;
	while (*traversal != NULL)
		printf("%s\n", *(traversal++));
}
