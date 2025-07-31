/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_expandable_part.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:33:30 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:31:42 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

char	*handle_quote_char(char *result, char quote_char)
{
	char	quote_str[2];
	char	*temp;

	quote_str[0] = quote_char;
	quote_str[1] = '\0';
	temp = ft_strjoin(result, quote_str);
	free(result);
	return (temp);
}

static char	*process_expandable_segment(char *result, char *str, int *range,
		t_shell *shell)
{
	char	*segment;
	char	*expanded;
	char	*temp;

	segment = ft_substr(str, range[0], range[1] - range[0]);
	if (!segment)
	{
		free(result);
		return (NULL);
	}
	expanded = expandvar(segment, shell);
	free(segment);
	if (expanded)
		temp = ft_strjoin(result, expanded);
	else
		temp = ft_strjoin(result, "");
	free(result);
	free(expanded);
	return (temp);
}

char	*handle_segment_processing(char *result, char *str, int *vars,
		t_shell *shell)
{
	int		range[2];
	char	*segment;
	char	*temp;

	range[0] = vars[1];
	range[1] = vars[0];
	if (!vars[2])
		return (process_expandable_segment(result, str, range, shell));
	else
	{
		segment = ft_substr(str, range[0], range[1] - range[0]);
		if (!segment)
		{
			free(result);
			return (NULL);
		}
		temp = ft_strjoin(result, segment);
		free(result);
		free(segment);
		return (temp);
	}
}
