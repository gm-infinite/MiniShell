/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_and_or.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 19:15:24 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/19 11:45:37 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

// Cuts out the outermost (). New string == empty? Remove it from 'split'
void	cut_out_par(t_split *split)
{
	char	*start;
	char	*end;

	start = split->start[0];
	end = split->start[split->size - 1];
	if (start == end && (size_t)(countchr_str(start, '(') + \
		countchr_str(start, ')')) == ft_strlen(start))
	{
		split->size--;
		return ;
	}
	if (start[0] == '(' && end[ft_strlen(end) - 1] == ')')
	{
		end[ft_strlen(end) - 1] = '\0';
		ft_memmove(&(start[0]), &(start[1]), ft_strlen(start));
	}
	if (split->size > 1 && start[0] == '\0')
	{
		split->start = &(split->start[1]);
		split->size--;
	}
	if (end[0] == '\0')
		split->size--;
}

char	*get_cut_indexs(t_split split)
{
	int		i;
	int		par;
	char	*ret;
	int		check;

	i = -1;
	par = 0;
	check = 0;
	ret = ft_calloc(count_str_split(split, "||", 1) + \
	count_str_split(split, "&&", 1) + 1, sizeof(char));
	if (ret == NULL)
		return (NULL);
	while (++i < split.size)
	{
		par += countchr_not_quote(split.start[i], '(');
		if (par == 0)
		{
			if (ft_strncmp("||", split.start[i], 3) == 0)
				ret[check++] = '1';
			else if (ft_strncmp("&&", split.start[i], 3) == 0)
				ret[check++] = '0';
		}
		par -= countchr_not_quote(split.start[i], ')');
	}
	return (ret);
}

void	parse_and_or(t_shell *shell, t_split split, char *c_i)
{
	int	i;
	int	par;
	int	st;
	int	check;

	i = -1;
	par = 0;
	st = 0;
	check = -2;
	while (++i <= split.size)
	{
		if (i < split.size)
			par += countchr_not_quote(split.start[i], '(');
		if (par == 0 && (i == split.size || ft_strncmp("&&", split.start[i], \
			3) == 0 || ft_strncmp("||", split.start[i], 3) == 0))
		{
			if (i < split.size && split.start[i] != NULL)
				free(split.start[i]);
			if (i < split.size)
				split.start[i] = NULL;
			if (++check == -1 || (c_i[check] == '0' && shell->past_exit_status \
			== 0) || (c_i[check] == '1' && shell->past_exit_status != 0))
				parser_and_or(shell, create_split(&(split.start[st]), i - st));
			st = i + 1;
		}
		if (i < split.size)
			par -= countchr_not_quote(split.start[i], ')');
	}
}

int	paranthesis_parity_check(t_split split)
{
	int	i;
	int j;
	int	par;
	int	quote_state;

	i = 0;
	par = 0;
	while (i < split.size)
	{
		j = 0;
		quote_state = 0;
		while (split.start[i][j] != '\0')
		{
			if (split.start[i][j] == '\'' && !(quote_state & 2))
				quote_state ^= 1;
			else if (split.start[i][j] == '"' && !(quote_state & 1))
				quote_state ^= 2;
			else if (!quote_state)
				par += (split.start[i][j] == '(') - (split.start[i][j] == ')');
			j++;
		}
		i++;
	}
	if (par == 0)
		return (1);
	return (0);
}

// Check for syntax errors in logical operators
int	check_operator_syntax_errors(t_split split)
{
	int i;
	
	// Check for incomplete operators at the end
	if (split.size > 0)
	{
		char *last_token = split.start[split.size - 1];
		if (ft_strncmp(last_token, "&&", 3) == 0 || ft_strncmp(last_token, "||", 3) == 0)
		{
			write(STDERR_FILENO, "bash: syntax error: unexpected end of file\n", 44);
			return (0);  // Syntax error found
		}
	}
	
	// Check for consecutive operators
	for (i = 0; i < split.size - 1; i++)
	{
		if ((ft_strncmp(split.start[i], "&&", 3) == 0 || ft_strncmp(split.start[i], "||", 3) == 0) &&
			(ft_strncmp(split.start[i + 1], "&&", 3) == 0 || ft_strncmp(split.start[i + 1], "||", 3) == 0))
		{
			write(STDERR_FILENO, "bash: syntax error near unexpected token `", 42);
			write(STDERR_FILENO, split.start[i + 1], ft_strlen(split.start[i + 1]));
			write(STDERR_FILENO, "'\n", 2);
			return (0);  // Syntax error found
		}
	}
	
	return (1);  // No syntax errors
}

// Check for parentheses syntax errors  
int	check_parentheses_syntax_errors(t_split split)
{
	int i;
	
	// Check for empty parentheses within tokens or adjacent tokens
	for (i = 0; i < split.size; i++)
	{
		// Check for "()" within a single token
		if (ft_strnstr(split.start[i], "()", ft_strlen(split.start[i])))
		{
			write(STDERR_FILENO, "bash: syntax error near unexpected token `)'\n", 45);
			return (0);
		}
		
		// Check for consecutive "(" ")" tokens  
		if (i < split.size - 1)
		{
			if (ft_strncmp(split.start[i], "(", 2) == 0 && ft_strncmp(split.start[i + 1], ")", 2) == 0)
			{
				write(STDERR_FILENO, "bash: syntax error near unexpected token `)'\n", 45);
				return (0);
			}
		}
	}
	
	// Check for adjacent parenthetical groups
	for (i = 0; i < split.size - 1; i++)
	{
		// Look for ) followed by (
		if (ft_strchr(split.start[i], ')') && ft_strchr(split.start[i + 1], '('))
		{
			write(STDERR_FILENO, "bash: syntax error near unexpected token `('\n", 45);
			return (0);
		}
		
		// Look for ) followed by command
		if (ft_strchr(split.start[i], ')') && split.start[i + 1] && 
			!ft_strchr(split.start[i + 1], '(') && 
			ft_strncmp(split.start[i + 1], "&&", 3) != 0 && 
			ft_strncmp(split.start[i + 1], "||", 3) != 0)
		{
			write(STDERR_FILENO, "bash: syntax error near unexpected token `", 42);
			write(STDERR_FILENO, split.start[i + 1], ft_strlen(split.start[i + 1]));
			write(STDERR_FILENO, "'\n", 2);
			return (0);
		}
	}
	
	return (1);  // No syntax errors
}

// Parser for && and || with () priority
void	parser_and_or(t_shell *shell, t_split split)
{
	char	*cut_indexs;

	if (split.size <= 0 || split.start == NULL)
		return ;
		
	// Check for operator syntax errors first
	if (!check_operator_syntax_errors(split))
	{
		shell->past_exit_status = 2;  // Bash uses exit code 2 for syntax errors
		return ;
	}
	
	// Check for parentheses syntax errors
	if (!check_parentheses_syntax_errors(split))
	{
		shell->past_exit_status = 2;  // Bash uses exit code 2 for syntax errors
		return ;
	}
	
	if (paranthesis_parity_check(split) == 0)
	{
		write(STDERR_FILENO, "bash: syntax error: unexpected end of file\n", 44);
		shell->past_exit_status = 2;
		return ;
	}
	if (check_single_par(split) != 0)
	{
		cut_out_par(&split);
		parser_and_or(shell, split);
		return ;
	}
	if (count_str_split(split, "||", 1) + count_str_split(split, "&&", 1) > 0)
	{
		cut_indexs = get_cut_indexs(split);
		parse_and_or(shell, split, cut_indexs);
		free(cut_indexs);
	}
	else
		shell->past_exit_status = execute_command(split, shell);
}
