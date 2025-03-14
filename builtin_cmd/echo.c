/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 11:50:52 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/03/14 14:35:10 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int echo_n_flag_check(t_split split)
{
	int i;
	int j;

	i = 0;
	while (i <= split.size)
	{
		j = 1;
		if (split.start[i][0] != '-')
			break;
		while (split.start[i][j] != '\0')
		{
			if (split.start[i][j] != 'n')
			{
				j = -1;
				break;
			}
			j++;
		}
		if (j == -1)
			return i;
		i++;
	}
	return i;
}

int echo(t_split split)
{
	int indx;
	int nflag;

	nflag = 0;
	indx = echo_n_flag_check(split);
	if (indx != 0)
		nflag = 1;
	while (indx < split.size)
	{
		printf("%s", split.start[indx]);
		if (indx < split.size - 1)
			printf(" ");
		indx++;
	}
	if (nflag == 0)
		printf("\n");
	return (0);
}

int main()
{
	char *a[] = {"-nnnn", "-nnnnk", "hello", "-n"};
	echo(create_split(a, 4));
}
