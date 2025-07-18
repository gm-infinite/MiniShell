/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 22:20:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/18 21:41:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_new.h"

int	check_quotes(char *str)
{
	int	i = 0;
	int	single_open = 0;
	int	double_open = 0;

	if (!str)
		return (1);

	while (str[i])
	{
		if (str[i] == '\'' && !double_open)
			single_open = !single_open;
		else if (str[i] == '\"' && !single_open)
			double_open = !double_open;
		i++;
	}

	return (!single_open && !double_open);
}

char	*process_quotes(char *str, t_shell *shell)
{
	char	*result;
	int		i = 0;
	int		j = 0;
	int		len = ft_strlen(str);
	int		in_single = 0;
	int		in_double = 0;

	if (!str)
		return (NULL);

	result = malloc(len + 1);
	if (!result)
		return (NULL);

	while (str[i])
	{
		if (str[i] == '\'' && !in_double)
		{
			in_single = !in_single;
		}
		else if (str[i] == '\"' && !in_single)
		{
			in_double = !in_double;
		}
		else
		{
			if (str[i] == '$' && in_double && !in_single)
			{
				// Handle variable expansion in double quotes
				if (str[i + 1] == '?')
				{
					char *exit_status = ft_itoa(shell->past_exit_status);
					int k = 0;
					while (exit_status[k])
						result[j++] = exit_status[k++];
					free(exit_status);
					i++; // Skip the '?'
				}
				else if (ft_isalnum(str[i + 1]) || str[i + 1] == '_')
				{
					// Extract variable name
					int start = i + 1;
					while (str[i + 1] && (ft_isalnum(str[i + 1]) || str[i + 1] == '_'))
						i++;
					
					char *var_name = ft_substr(str, start, (i + 1) - start);
					char *var_value = get_env_value(var_name, shell);
					if (var_value)
					{
						int k = 0;
						while (var_value[k])
							result[j++] = var_value[k++];
					}
					free(var_name);
				}
				else
				{
					result[j++] = str[i];
				}
			}
			else
			{
				result[j++] = str[i];
			}
		}
		i++;
	}

	result[j] = '\0';
	return (result);
}
