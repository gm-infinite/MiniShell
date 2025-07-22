/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_arg_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 23:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 23:46:49 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../parser/wildcard_handle/wildcard_handler.h"

int	is_empty_or_whitespace(char *str)
{
	int	k;

	k = 0;
	while (str[k] && (str[k] == ' ' || str[k] == '\t'))
		k++;
	return (str[k] == '\0');
}

int	count_non_empty_args(char **args)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
			count++;
		i++;
	}
	return (count);
}

int	copy_non_empty_args(char **args, char **filtered)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
		{
			filtered[j] = ft_strdup(args[i]);
			if (!filtered[j])
				return (-1);
			j++;
		}
		i++;
	}
	return (j);
}

void	process_and_check_args(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	if (!args[0])
	{
		free_args(args);
		exit(0);
	}
	i = -1;
	while (args[++i])
	{
		expanded = expand_variables_quoted(args[i], shell);
		if (expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
	}
	process_args_quotes(args, shell);
	if (!args[0])
		handle_empty_pipe_args(args);
}

void	handle_empty_pipe_args(char **args)
{
	if (args[0] && args[0][0] == '\0')
		write(STDERR_FILENO, ": command not found\n", 20);
	free_args(args);
	if (args[0] && args[0][0] == '\0')
		exit(127);
	else
		exit(0);
}

void	expand_wildcards_in_args(char **args)
{
	char	*temp_input;
	char	*wildcard_expanded;
	t_split	expanded_split;
	int		i;

	if (!args || !args[0])
		return ;
	temp_input = NULL;
	i = 0;
	while (args[i])
	{
		if (temp_input == NULL)
			temp_input = ft_strdup(args[i]);
		else
		{
			char *temp = ft_strjoin(temp_input, " ");
			free(temp_input);
			temp_input = temp;
			temp = ft_strjoin(temp_input, args[i]);
			free(temp_input);
			temp_input = temp;
		}
		i++;
	}

	if (!temp_input)
		return ;
	wildcard_expanded = wildcard_input_modify(temp_input);
	free(temp_input);

	if (!wildcard_expanded || wildcard_expanded == temp_input)
	{
		if (wildcard_expanded && wildcard_expanded != temp_input)
			free(wildcard_expanded);
		return ;
	}
	expanded_split = create_split_str(wildcard_expanded);
	free(wildcard_expanded);
	if (expanded_split.size == 0)
	{
		free_split(&expanded_split);
		return ;
	}
	i = 0;
	while (i < expanded_split.size && args[i])
	{
		char *unquoted = remove_quotes_from_string(expanded_split.start[i]);
		free(args[i]);
		args[i] = unquoted ? unquoted : ft_strdup(expanded_split.start[i]);
		i++;
	}
	while (args[i])
	{
		free(args[i]);
		args[i] = NULL;
		i++;
	}
	free_split(&expanded_split);
}
