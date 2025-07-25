/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_tilde.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 00:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 18:41:56 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*tilde(char *str, t_shell *shell)
{
	char	*home;

	if (!str || str[0] != '~')
		return (ft_strdup(str));
	if (str[0] == '~' && (str[1] == '\0' || str[1] == '/'))
	{
		home = get_env_value("HOME", shell);
		if (home)
		{
			if (str[1] == '\0')
				return (ft_strdup(home));
			else
				return (ft_strjoin(home, &str[1]));
		}
	}
	return (ft_strdup(str));
}
