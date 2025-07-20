/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes_new.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:00:00 by user              #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	check_quotes(char *str)
{
	int	i;
	int	single_open;
	int	double_open;

	if (!str)
		return (1);
	i = 0;
	single_open = 0;
	double_open = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !double_open)
			single_open = !single_open;
		else if (str[i] == '"' && !single_open)
			double_open = !double_open;
		i++;
	}
	return (single_open == 0 && double_open == 0);
}

static int	is_entirely_single_quoted(char *str)
{
	int	quote_count;
	int	k;

	if (str[0] != '\'' || str[ft_strlen(str) - 1] != '\'')
		return (0);
	quote_count = 0;
	k = 0;
	while (str[k])
	{
		if (str[k] == '\'')
			quote_count++;
		k++;
	}
	return (quote_count == 2);
}

static char	*remove_quotes_from_string(char *str)
{
	char	*result;
	int		i;
	int		j;
	int		in_single;
	int		in_double;

	result = malloc(ft_strlen(str) * 2 + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	in_single = 0;
	in_double = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (str[i] == '"' && !in_single)
			in_double = !in_double;
		else
		{
			result[j] = str[i];
			j++;
		}
		i++;
	}
	result[j] = '\0';
	return (result);
}

char	*process_quotes(char *str, t_shell *shell)
{
	char	*result;
	char	*expanded;

	if (!str)
		return (NULL);
	result = remove_quotes_from_string(str);
	if (!result)
		return (NULL);
	if (is_entirely_single_quoted(str))
		return (result);
	expanded = expand_variables(result, shell);
	if (expanded)
	{
		free(result);
		return (expanded);
	}
	return (result);
}

void	process_args_quotes(char **args, t_shell *shell)
{
	int		i;
	char	*processed;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		processed = process_quotes(args[i], shell);
		if (processed)
		{
			free(args[i]);
			args[i] = processed;
		}
		i++;
	}
}
