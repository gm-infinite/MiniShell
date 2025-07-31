/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_args_quotes.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:30:21 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:27:42 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

static int	process_quote_character(char *str, char *result,
	int *indices, int *states)
{
	int	i;
	int	j;

	i = indices[0];
	j = indices[1];
	if (handle_escaped_quote(str, result, i, j))
	{
		indices[1] = j + 1;
		indices[0] = i + 2;
		return (1);
	}
	if (should_copy_character(str[i], states))
	{
		result[j] = str[i];
		indices[1] = j + 1;
	}
	update_quote_states(str[i], states);
	indices[0] = i + 1;
	return (0);
}

static char	*remove_quotes_from_string(char *str)
{
	char	*result;
	int		indices[2];
	int		states[2];

	result = malloc(ft_strlen(str) * 2 + 1);
	if (!result)
		return (NULL);
	indices[0] = 0;
	indices[1] = 0;
	states[0] = 0;
	states[1] = 0;
	while (str[indices[0]])
		if (process_quote_character(str, result, indices, states))
			continue ;
	result[indices[1]] = '\0';
	return (result);
}

static char	*process_quotes(char *str, t_shell *shell)
{
	char	*result;
	char	*expanded;

	if (!str)
		return (NULL);
	expanded = expandvar_q(str, shell);
	if (!expanded)
		return (NULL);
	result = remove_quotes_from_string(expanded);
	if (!result)
		result = expanded;
	else
	{
		if (expanded != str)
			free(expanded);
	}
	return (result);
}

void	process_args_quotes(char **args, t_shell *shell)
{
	int		i;
	char	*processed;

	if (!args)
		return ;
	i = -1;
	while (args[++i])
	{
		processed = process_quotes(args[i], shell);
		if (processed)
		{
			free(args[i]);
			args[i] = processed;
		}
	}
}
