/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 21:06:16 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/24 21:21:42 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../main/minishell.h"
#include "../e-libft/libft.h"

int	handle_executable_validation(char **args, char *executable)
{
	if (!executable)
		return (handle_executable_not_found(args));
	if (ft_strncmp(executable, "__NOT_DIRECTORY__", 17) == 0)
	{
		write_not_directory_error(args[0]);
		free(executable);
		return (126);
	}
	return (validate_executable(args[0], executable));
}
