/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_wildcard_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 23:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 23:46:49 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../parser/wildcard_handle/wildcard_handler.h"

void	handle_empty_pipe_args(char **args)
{
	if (args[0] && args[0][0] == '\0')
		write(STDERR_FILENO, ": command not found\n", 20);
	free_args(args);
	if (args[0] && args[0][0] == '\0')
		exit(127);
	else
		exit(0);
}
