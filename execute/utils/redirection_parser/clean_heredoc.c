/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:52:55 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:29:39 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

int	clean_heredoc(char *processed_filename, t_redir_fds *fds)
{
	*fds->input_fd = open(processed_filename, O_RDONLY);
	if (*fds->input_fd == -1)
		return (-1);
	unlink(processed_filename);
	return (0);
}
