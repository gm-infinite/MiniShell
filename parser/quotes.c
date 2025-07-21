/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:00:00 by user              #+#    #+#             */
/*   Updated: 2025/07/20 19:57:43 by emgenc           ###   ########.fr       */
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

char	*process_quotes(char *str, t_shell *shell)
{
	char	*result;
	char	*expanded;

	if (!str)
		return (NULL);
	
	// Use the new quote-aware expansion function
	expanded = expand_with_quotes(str, shell);
	if (!expanded)
		return (NULL);
	
	// Then remove the quotes
	result = remove_quotes_from_string(expanded);
	if (!result)
		result = expanded;
	else if (expanded != str)
		free(expanded);
	
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
