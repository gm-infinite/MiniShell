/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 13:52:11 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 12:57:08 by emgenc           ###   ########.fr       */
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

int	validate_redirection_syntax(t_split split)
{
    int i;
    int has_parentheses;
    int err;
    int redirect_type;

    i = -1;
    has_parentheses = has_parentheses_in_split(split);
    while (++i < split.size)
    {
        redirect_type = is_redirection(split.start[i]);
        if (redirect_type)
        {
            err = check_redirection_error(split, i);
            if (err)
                return err;
            i += 1;
            continue;
        }
        if (ft_strncmp(split.start[i], "|", 2) == 0 && ft_strlen(split.start[i]) == 1)
        {
            err = check_pipe_error(split, i, has_parentheses);
            if (err)
                return err;
        }
    }
    return 0;
}
