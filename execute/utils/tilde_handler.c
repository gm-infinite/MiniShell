/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tilde_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:28:43 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 13:28:36 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

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
