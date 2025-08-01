/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_expander.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:00:56 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/31 20:05:33 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

static char	*handle_double_quote_case(char *result, int *i,
		int *in_double_quotes, int in_single_quotes)
{
	if (!in_single_quotes)
	{
		result = handle_quote_char(result, '"');
		*in_double_quotes = !*in_double_quotes;
		(*i)++;
		return (result);
	}
	return (NULL);
}

static char	*handle_single_quote_case(char *result, int *i,
		int *in_single_quotes, int in_double_quotes)
{
	if (!in_double_quotes)
	{
		result = handle_quote_char(result, '\'');
		*in_single_quotes = !*in_single_quotes;
		(*i)++;
		return (result);
	}
	return (NULL);
}

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
	result = ft_calloc(1, 1);
	if (!result)
		return (NULL);
	vars[0] = 0;
	vars[1] = 0;
	vars[2] = 0;
	vars[3] = 0;
	return (process_quote_expansion_loop(str, shell, result, vars));
}
