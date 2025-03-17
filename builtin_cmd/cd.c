/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 11:28:42 by emgenc            #+#    #+#             */
/*   Updated: 2025/03/17 11:02:55 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include "../t_split_utils/t_split.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>

// FUNCTION PROTOTYPE FORWARD DECLARATION: IS THIS ALLOWED?
// I THINK IT IS ALLOWED, BUT I AM NOT SURE.
int	export(char *to_be_exported, char ***envp);

void	cd(char *argv[], char **envp)
{
	char	*oldpwd;
	char	*newpwd;
	char	*target;

	oldpwd = ft_strjoin("OLDPWD=", getcwd(NULL, 0));
	if (!oldpwd)
		return (perror("getcwd"));
	if (!argv[1])
		target = getenv("HOME");
	else
		target = argv[1];
	if (!target)
		return (fprintf(stderr, "cd: HOME not set\n"), free(oldpwd));
	if (chdir(target) != 0)
		return (perror("cd"), free(oldpwd));
	newpwd = ft_strjoin("PWD=", getcwd(NULL, 0));
	if (!newpwd)
		return (perror("getcwd"), free(oldpwd));
	export(oldpwd, &envp);
	export(newpwd, &envp);
	free(oldpwd);
	free(newpwd);
}
