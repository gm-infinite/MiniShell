/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 11:50:52 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/03/16 18:04:32 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static int echo_n_flag_check(char **arg)
{
	int i;
	int j;

	i = 0;
	while (arg[i] != NULL)
	{
		j = 1;
		if (arg[i][0] != '-')
			break;
		while (arg[i][j] != '\0')
		{
			if (arg[i][j] != 'n')
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

int echo(char **arg)
{
	int indx;
	int nflag;

	nflag = 0;
	indx = echo_n_flag_check(arg);
	if (indx != 0)
		nflag = 1;
	while (arg[indx] != NULL)
	{
		printf("%s", arg[indx]);
		if (arg[indx + 1] != NULL)
			printf(" ");
		indx++;
	}
	if (nflag == 0)
		printf("\n");
	return (0);
}

