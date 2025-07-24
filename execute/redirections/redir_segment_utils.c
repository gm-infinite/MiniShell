/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_segment_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:52:01 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

char	*join_expanded_segment(char *result, char *expanded)
{
	char	*temp;

	if (expanded)
		temp = ft_strjoin(result, expanded);
	else
		temp = ft_strjoin(result, "");
	free(result);
	free(expanded);
	return (temp);
}

static char	*expand_segment(char *result, char *str, int start, int end)
{
	char	*segment;
	char	*temp;

	segment = ft_substr(str, start, end - start);
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

static char	*process_expandable_segment(char *result, char *str, int *range,
		t_shell *shell)
{
	char	*segment;
	char	*expanded;

	segment = ft_substr(str, range[0], range[1] - range[0]);
	if (!segment)
	{
		free(result);
		return (NULL);
	}
	expanded = expand_variables(segment, shell);
	free(segment);
	return (join_expanded_segment(result, expanded));
}

char	*handle_segment_processing(char *result, char *str, int *vars,
		t_shell *shell)
{
	int	range[2];

	range[0] = vars[1];
	range[1] = vars[0];
	if (!vars[2])
		return (process_expandable_segment(result, str, range, shell));
	else
		return (expand_segment(result, str, vars[1], vars[0]));
}
