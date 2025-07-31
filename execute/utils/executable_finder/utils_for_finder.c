/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_for_finder.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:48:15 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:18:10 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

void	write_path_error_message(char *cmd, char *message)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, message, ft_strlen(message));
}

int	is_special_directory(char *cmd)
{
	if ((ft_strncmp(cmd, "..", 3) == 0 && ft_strlen(cmd) == 2)
		|| (ft_strncmp(cmd, ".", 2) == 0 && ft_strlen(cmd) == 1))
		return (1);
	return (0);
}
