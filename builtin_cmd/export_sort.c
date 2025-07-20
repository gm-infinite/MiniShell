/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_sort.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 16:15:03 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

static void	swap_env_ptrs(char **a, char **b)
{
	char	*temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

static void	sort_env_array(char **sorted_env, int count)
{
	int	i;
	int	j;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strncmp(sorted_env[j], sorted_env[j + 1],
					ft_strlen(sorted_env[j])) > 0)
				swap_env_ptrs(&sorted_env[j], &sorted_env[j + 1]);
			j++;
		}
		i++;
	}
}

static int	get_env_count(t_shell *shell)
{
	int	count;

	count = 0;
	while (shell->envp[count])
		count++;
	return (count);
}

int	print_sorted_env(t_shell *shell)
{
	char	**sorted_env;
	int		count;
	int		i;

	count = get_env_count(shell);
	sorted_env = malloc(sizeof(char *) * count);
	if (!sorted_env)
		return (1);
	i = 0;
	while (i < count)
	{
		sorted_env[i] = shell->envp[i];
		i++;
	}
	sort_env_array(sorted_env, count);
	i = 0;
	while (i < count)
	{
		printf("declare -x %s\n", sorted_env[i]);
		i++;
	}
	free(sorted_env);
	return (0);
}
