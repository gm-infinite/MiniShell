/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 11:28:42 by emgenc            #+#    #+#             */
/*   Updated: 2025/03/15 17:52:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <errno.h>
#include "../main/minishell.h"
#include "../t_split_utils/t_split.h"

static int	modifyenv(char **iterate, char *envvar, char *value)
{
	char	*initial_iterate;
	char	*first_part;

	initial_iterate = *iterate;
	first_part = ft_strjoin(envvar, "=");
	*iterate = ft_strjoin(first_part, value);
	free(first_part);
	free(initial_iterate);
	return (0);
}

static int	realloc_createenv(char ***env, unsigned long long envamount, char *newent)
{
	char				**new_env;
	unsigned long long	i;

	/* Allocate space for (envamount + 1 new variable + 1 NULL terminator) pointers */
	new_env = malloc(sizeof(char *) * (envamount + 2));
	if (!new_env)
		return (-1);
	/* Copy the existing environment pointers PLUS the new environment variable, and terminate the array. */
	i = 0;
	while (i < envamount)
		new_env[i] = (*env)[i++];
	new_env[envamount] = newent;
	new_env[envamount + 1] = NULL;

	/* Free the old array and assign the pointer to the new one*/
	free(*env);
	*env = new_env;
	return (0);
}

static int	e_setenv(char *envvar, char *value, char **allvars)
{
	char				**iterate;
	unsigned long long	envamount;
	char				*completed_envvar;
	char				*first_part1;

	iterate = allvars;
	envamount = 0;
	while (*iterate != NULL)
	{
		envamount++;
		iterate++;
	}
	iterate = allvars;
	while (*iterate != NULL
		&& ft_strncmp(*iterate, envvar, ft_strlen(envvar)) != 0)
		iterate++;
	if (*iterate != NULL)
		return(modifyenv(iterate, envvar, value), 0);
	else if (ft_strncmp(*iterate, envvar, ft_strlen(envvar)) == 0)
	{
		first_part1 = ft_strjoin(envvar, "=");
		completed_envvar = ft_strjoin(first_part1, value);
		realloc_createenv(&allvars, envamount, completed_envvar);
		return(free(first_part1), 0);
	}
    return (-1);
}

void	cd(char **envp)
{
	char	*target;
	char	*oldpwd;
	char	*newpwd;

	/* This saves current directory.
	   We need to do this, because it seems like 
	   Bash creates an environment variable named "OLDPWD" 
	   after changing a directory.
	   Thank you ChatGPT for the heads-up warning! */
	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
		return (perror("getcwd"));
	/* Determine target directory 
	   If "cd" has been called alone without arguments, 
	   it means that the shell should switch to $HOME. */
	if (!envp[1])
	{
		target = getenv("HOME");
		if (!target)
			return (fprintf(2, "cd: HOME not set\n"), free(oldpwd));
	}
	else
		target = envp[1];
	/* * CHANGE  DIRECTORY and get new cwd afterwards* */
	if (chdir(target) != 0)
		return (perror("cd"), free(oldpwd));
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		return (perror("getcwd"), free(oldpwd));
	/* Update environment variables.
	   Normally this is done as:
	   
	   setenv("OLDPWD", oldpwd);
	   setenv("PWD", newpwd); */
	e_setenv("OLDPWD", oldpwd, envp);
	e_setenv("PWD", newpwd, envp);
	free(oldpwd);
	free(newpwd);
}



