/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_wildcard_expand_utils.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 23:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 23:46:49 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../parser/wildcard_handle/wildcard_handler.h"

int	validate_and_expand_input(char *temp_input, char **wildcard_expanded)
{
	if (!temp_input)
		return (0);
	*wildcard_expanded = wildcard_input_modify(temp_input);
	free(temp_input);
	if (!*wildcard_expanded || *wildcard_expanded == temp_input)
	{
		if (*wildcard_expanded && *wildcard_expanded != temp_input)
			free(*wildcard_expanded);
		return (0);
	}
	return (1);
}

int	prepare_expanded_split(char *wildcard_expanded, t_split *expanded_split)
{
	*expanded_split = create_split_str(wildcard_expanded);
	free(wildcard_expanded);
	if (expanded_split->size == 0)
	{
		free_split(expanded_split);
		return (0);
	}
	return (1);
}
