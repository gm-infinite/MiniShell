/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 14:23:40 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/24 14:23:57 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

void	write_heredoc_warning(char *processed_delimiter)
{
	write(STDERR_FILENO, "minishell: warning: here-document delimited by "
		"end-of-file (wanted `", 68);
	write(STDERR_FILENO, processed_delimiter, ft_strlen(processed_delimiter));
	write(STDERR_FILENO, "')\n", 3);
}