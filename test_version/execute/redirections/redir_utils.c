/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:52:11 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 13:11:49 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	is_redirection(char *token)
{
	if (!token)
		return (0);
	if (ft_strncmp(token, "<<", 3) == 0 && ft_strlen(token) == 2)
		return (1);
	if (ft_strncmp(token, ">>", 3) == 0 && ft_strlen(token) == 2)
		return (2);
	if (ft_strncmp(token, "<", 2) == 0 && ft_strlen(token) == 1)
		return (3);
	if (ft_strncmp(token, ">", 2) == 0 && ft_strlen(token) == 1)
		return (4);
	return (0);
}
