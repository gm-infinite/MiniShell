/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:15:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 20:17:42 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	handle_home_path(char **path, char **home, t_shell *shell)
{
	*home = get_env_value("HOME", shell);
	if (!(*home))
	{
		write(STDERR_FILENO, "cd: HOME not set\n", 17);
		return (1);
	}
	*path = *home;
	return (0);
}

int	handle_oldpwd_path(char **path, t_shell *shell)
{
	*path = get_env_value("OLDPWD", shell);
	if (!(*path))
	{
		write(STDERR_FILENO, "cd: OLDPWD not set\n", 19);
		return (1);
	}
	printf("%s\n", *path);
	return (0);
}
