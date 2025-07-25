/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:34:56 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/23 10:51:48 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	builtin_env(t_shell *shell)
{
	int		i;
	char	*eq_pos;

	i = -1;
	while (shell->envp[++i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos)
			printf("%s\n", shell->envp[i]);
	}
	return (0);
}
