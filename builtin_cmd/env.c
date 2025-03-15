/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 12:45:02 by emgenc            #+#    #+#             */
/*   Updated: 2025/03/15 18:02:51 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "../main/minishell.h"
#include "../t_split_utils/t_split.h"

char	*env(char **envp)
{
	/* THIS IS FOR PRINTING ALL ENVIRONMENT VARIABLES "ONLY" !!! for now.
	#include <unistd.h>
	#include "../main/minishell.h"
	#include "../t_split_utils/t_split.h"
	
	void	envdir(char **envp)
	{
		char **traversal;
	
		traversal = envp;
		while (*traversal != NULL)
			printf("%s\n", *(traversal++));
	}

	int main(int argc, char *argv[], char **envp) {
		(void)argc;
		(void)argv;
		envdir(envp);
	}
	*/
}
