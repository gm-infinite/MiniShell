/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 12:53:07 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 12:58:25 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

int	has_parentheses_in_split(t_split split)
{
    int i;
    for (i = 0; i < split.size; i++)
        if (ft_strchr(split.start[i], '(') || ft_strchr(split.start[i], ')'))
            return 1;
    return 0;
}

int	check_pipe_error(t_split split, int i, int has_parentheses)
{
    if (!has_parentheses && (i == 0 || i == split.size - 1))
    {
        write(STDERR_FILENO, "bash: syntax error near unexpected token `|'\n", 45);
        return 2;
    }
    return 0;
}

int	check_redirection_error(t_split split, int i)
{
    int filename_index = i + 1;
    if (filename_index >= split.size)
    {
        write(STDERR_FILENO, "bash: syntax error near unexpected token `newline'\n", 52);
        return 2;
    }
    char *next_token = split.start[filename_index];
    if (is_redirection(next_token) || ft_strncmp(next_token, "|", 2) == 0)
    {
        write(STDERR_FILENO, "bash: syntax error near unexpected token `", 42);
        write(STDERR_FILENO, next_token, ft_strlen(next_token));
        write(STDERR_FILENO, "'\n", 2);
        return 2;
    }
    return 0;
}

char	*remove_quotes_for_redirection(char *str)
{
	char			*result;
	int				i;
	int				j;
	unsigned char	quote_state;
	int				len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	quote_state = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !(quote_state & 2))
			quote_state ^= 1;
		else if (str[i] == '"' && !(quote_state & 1))
			quote_state ^= 2;
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

