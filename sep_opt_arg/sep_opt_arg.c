/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sep_opt_arg.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 21:16:17 by kuzyilma          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2025/03/22 14:05:35 by kuzyilma         ###   ########.fr       */
=======
/*   Updated: 2025/03/21 22:19:37 by kuzyilma         ###   ########.fr       */
>>>>>>> 54b61d2 (started on seperating some operation and arguments)
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

<<<<<<< HEAD
static const char *get_opt(int indx)
=======
const char *get_opt(int indx)
>>>>>>> 54b61d2 (started on seperating some operation and arguments)
{
	static char *operator[] = {"||", "&&", "<<", ">>", "|", ">", "<"};

	return (operator[indx]);
}

<<<<<<< HEAD
static int get_inquote(char *un_sep, int size)
=======
int get_inquote(char *un_sep, int size)
>>>>>>> 54b61d2 (started on seperating some operation and arguments)
{
	int i;
	int inquote;

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

<<<<<<< HEAD
static int get_sep_size(char *un_sep)
=======
int get_sep_size(char *un_sep)
>>>>>>> 54b61d2 (started on seperating some operation and arguments)
{
	int i;
	int extra;
	int j;

	i = 0;
	extra = 0;
	while (un_sep[i] != '\0')
	{
		j = 0;
		while (get_inquote(un_sep, i) == 0 && j < 7)
		{
			if (strncmp(&(un_sep[i]), get_opt(j), 1 + (j < 4)) == 0)
			{
				extra += (un_sep[i - 1] != ' ') + \
				(un_sep[i + 1 + (j < 4)] != ' ');
				break ;
			}
			j++;
		}
		i += 1 + (j < 4 && get_inquote(un_sep, i) == 0);
	}
	return (i + extra + 1);
}

<<<<<<< HEAD
static void set_sep(char *sep, char *un_sep, int sep_size)
=======
void set_sep(char *sep, char *un_sep, int sep_size)
>>>>>>> 54b61d2 (started on seperating some operation and arguments)
{
	int i;
	int offset;
	int j;

	i = 0;
	offset = 0;
	while (un_sep[i] != '\0')
	{
		j = -1;
		while (get_inquote(un_sep, i) == 0 && ++j < 7)
		{
			if (strncmp(&(un_sep[i]), get_opt(j), 1 + (j < 4)) == 0)
			{
				if (un_sep[i - 1] != ' ')
					sep[i + offset++] = ' ';
				ft_strlcat(sep, get_opt(j), sep_size);
				if (un_sep[i + 1 + (j < 4)] != ' ')
					sep[i + 1 + (j < 4) + offset++] = ' ';
				break ;
			}
		}
		if (j > 6 || get_inquote(un_sep, i) != 0)
			sep[i + offset] = un_sep[i];
		i += 1 + (j < 4 && get_inquote(un_sep, i) == 0);
	}
}

void sep_opt_arg(t_shell *shell)
{
	char *temp;
	int sep_size;

	temp = shell->current_input;
	sep_size = get_sep_size(temp);
<<<<<<< HEAD
	if ((size_t)sep_size == ft_strlen(shell->current_input))
		return ;
	shell->current_input = (char *)ft_calloc(sep_size, sizeof(char));
	set_sep(shell->current_input, temp, sep_size);
	free(temp);
}
=======
	if (sep_size == ft_strlen(shell->current_input))
		return ;
	shell->current_input = (char *)ft_calloc(sep_size, sizeof(char));
	set_sep(shell->current_input, temp, sep_size);
}

int main()
{
	char *un_sep = "kuz\"ey&&berk||hello|h\"i<o";
	int sep_size = get_sep_size(un_sep);
	char *sep = calloc(sep_size, sizeof(char));
	set_sep(sep, un_sep, sep_size);
	printf("%s\n", sep);
	free(sep);
}
>>>>>>> 54b61d2 (started on seperating some operation and arguments)
