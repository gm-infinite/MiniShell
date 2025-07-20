/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sep_opt_arg.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 21:16:17 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/20 21:23:16 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../main/minishell.h"

static const char	*get_opt(int indx)
{
	static char	*operator[] = {"||", "&&", "<<", ">>", "|", ">", "<"};

	return (operator[indx]);
}

static int	get_inquote(char *un_sep, int size)
{
	int		i;
	char	inquote;

	i = 0;
	inquote = 0;
	while (i <= size)
	{
		inquote ^= (un_sep[i] == '\"' && inquote != 2);
		inquote ^= 2 * (un_sep[i] == '\'' && inquote != 1);
		i++;
	}
	return (inquote);
}

static int	get_sep_size(char *un_sep)
{
	int	i;
	int	extra;
	int	j;

	i = 0;
	extra = 0;
	while (un_sep[i] != '\0')
	{
		j = 0;
		while (get_inquote(un_sep, i) == 0 && j < 7)
		{
			if (ft_strncmp(&(un_sep[i]), get_opt(j), 1 + (j < 4)) == 0)
			{
				extra += (i > 0 && un_sep[i - 1] != ' ') + \
				(un_sep[i + 1 + (j < 4)] != '\0' && un_sep[i + 1 + (j < 4)] != ' ');
				break ;
			}
			j++;
		}
		i += 1 + (j < 4 && get_inquote(un_sep, i) == 0);
	}
	return (i + extra + 1);
}

static void	set_sep(char *sep, char *un_sep, int sep_size)
{
	int	i;
	int	offset;
	int	j;

	i = 0;
	offset = 0;
	while (un_sep[i] != '\0')
	{
		j = -1;
		while (get_inquote(un_sep, i) == 0 && ++j < 7)
		{
			if (ft_strncmp(&(un_sep[i]), get_opt(j), 1 + (j < 4)) == 0)
			{
				if (i > 0 && un_sep[i - 1] != ' ')
					sep[i + offset++] = ' ';
				ft_strlcat(sep, get_opt(j), sep_size);
				if (un_sep[i + 1 + (j < 4)] != '\0' && un_sep[i + 1 + (j < 4)] != ' ')
					sep[i + 1 + (j < 4) + offset++] = ' ';
				break ;
			}
		}
		if (j > 6 || get_inquote(un_sep, i) != 0)
			sep[i + offset] = un_sep[i];
		i += 1 + (j < 4 && get_inquote(un_sep, i) == 0);
	}
}

void	sep_opt_arg(t_shell *shell)
{
	char	*temp;
	int		sep_size;

	temp = shell->current_input;
	sep_size = get_sep_size(temp);
	if ((size_t)sep_size == ft_strlen(shell->current_input) + 1)
		return ;
	shell->current_input = (char *)ft_calloc(sep_size, sizeof(char));
	set_sep(shell->current_input, temp, sep_size);
	free(temp);
}
