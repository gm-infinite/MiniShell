/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_quote_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/24 21:51:44 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static char	*process_quotes_and_expansions(char *str, char *result, int *vars)
{
	char	*temp_result;

	if (str[vars[0]] == '\'')
	{
		temp_result = handle_single_quote_case(result, &vars[0],
				&vars[2], vars[3]);
		if (temp_result)
			return (temp_result);
	}
	else if (str[vars[0]] == '"')
	{
		temp_result = handle_double_quote_case(result, &vars[0],
				&vars[3], vars[2]);
		if (temp_result)
			return (temp_result);
	}
	return (NULL);
}

static char	*process_quote_expansion_loop(char *str, t_shell *shell,
		char *result, int *vars)
{
	char	*temp_result;

	while (str[vars[0]])
	{
		vars[1] = vars[0];
		temp_result = process_quotes_and_expansions(str, result, vars);
		if (temp_result)
		{
			result = temp_result;
			continue ;
		}
		while (str[vars[0]] && ((str[vars[0]] != '\'' || vars[3])
				&& (str[vars[0]] != '"' || vars[2])))
			vars[0]++;
		if (vars[0] > vars[1])
		{
			result = handle_segment_processing(result, str, vars, shell);
			if (!result)
				return (NULL);
		}
	}
	return (result);
}

char	*process_q_expand(char *str, t_shell *shell)
{
	char	*result;
	int		vars[4];

	if (!str)
		return (NULL);
	result = init_expansion_result();
	if (!result)
		return (NULL);
	init_expansion_vars(vars);
	return (process_quote_expansion_loop(str, shell, result, vars));
}
