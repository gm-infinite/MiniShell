/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 22:15:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/18 21:41:43 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_new.h"

typedef struct s_expand
{
	char	*result;
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	char	*expanded;
	int		indx;
	int		var_len;
}	t_expand;

void	init_environment(t_shell *shell, char **envp)
{
	int	i = 0;
	int	count = 0;

	while (envp[count])
		count++;
	
	shell->envp = malloc(sizeof(char *) * (count + 1));
	if (!shell->envp)
		return;
	
	while (i < count)
	{
		shell->envp[i] = ft_strdup(envp[i]);
		i++;
	}
	shell->envp[i] = NULL;
}

char	*get_env_value(char *var_name, t_shell *shell)
{
	int		i = 0;
	int		len;
	char	*eq_pos;

	if (!var_name || !shell->envp)
		return (NULL);
	
	len = ft_strlen(var_name);
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
				return (eq_pos + 1);
		}
		i++;
	}
	return (NULL);
}

int	set_env_var(char *var_name, char *value, t_shell *shell)
{
	int		i = 0;
	int		len = ft_strlen(var_name);
	char	*eq_pos;
	char	**new_envp;
	char	*temp;

	if (!var_name || !value)
		return (1);

	// Check if variable already exists
	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
			{
				temp = ft_strjoin(var_name, "=");
				if (!temp)
					return (1);
				free(shell->envp[i]);
				shell->envp[i] = ft_strjoin(temp, value);
				free(temp);
				return (shell->envp[i] ? 0 : 1);
			}
		}
		i++;
	}

	// Add new variable
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (!new_envp)
		return (1);
	
	ft_memcpy(new_envp, shell->envp, sizeof(char *) * i);
	temp = ft_strjoin(var_name, "=");
	if (!temp)
	{
		free(new_envp);
		return (1);
	}
	new_envp[i] = ft_strjoin(temp, value);
	free(temp);
	if (!new_envp[i])
	{
		free(new_envp);
		return (1);
	}
	new_envp[i + 1] = NULL;
	free(shell->envp);
	shell->envp = new_envp;
	return (0);
}

int	unset_env_var(char *var_name, t_shell *shell)
{
	int		i = 0;
	int		j;
	int		len = ft_strlen(var_name);
	int		count = 0;
	char	*eq_pos;
	char	**new_envp;

	if (!var_name || !shell->envp)
		return (1);

	while (shell->envp[count])
		count++;

	while (shell->envp[i])
	{
		eq_pos = ft_strchr(shell->envp[i], '=');
		if (eq_pos && (eq_pos - shell->envp[i]) == len)
		{
			if (ft_strncmp(shell->envp[i], var_name, len) == 0)
			{
				new_envp = malloc(sizeof(char *) * count);
				if (!new_envp)
					return (1);
				
				j = 0;
				while (j < i)
				{
					new_envp[j] = shell->envp[j];
					j++;
				}
				free(shell->envp[i]);
				while (j < count - 1)
				{
					new_envp[j] = shell->envp[j + 1];
					j++;
				}
				new_envp[j] = NULL;
				
				free(shell->envp);
				shell->envp = new_envp;
				return (0);
			}
		}
		i++;
	}
	return (1);
}

void	free_environment(t_shell *shell)
{
	int	i = 0;
	
	if (!shell->envp)
		return;
	
	while (shell->envp[i])
	{
		free(shell->envp[i]);
		i++;
	}
	free(shell->envp);
	shell->envp = NULL;
}

static int	replace_var_with_value(t_expand *holder, t_shell *shell, int flag)
{
	char	*before;
	char	*after;
	char	*temp;
	
	(void)shell;
	before = ft_substr(holder->result, 0, holder->indx);
	if (flag == 1)
		after = ft_strdup(&holder->result[holder->indx + 2]);
	else
		after = ft_strdup(holder->var_end);
	
	temp = ft_strjoin(before, holder->var_value);
	holder->expanded = ft_strjoin(temp, after);
	
	free(before);
	free(after);
	free(temp);
	
	if (!holder->expanded)
	{
		if (flag == 0)
			free(holder->var_name);
		return (0);
	}
	
	holder->var_len = ft_strlen(holder->var_value);
	free(holder->result);
	if (flag == 0)
		free(holder->var_name);
	holder->result = holder->expanded;
	holder->indx += holder->var_len;
	return (1);
}

static int	handle_question_mark(t_expand *holder, t_shell *shell)
{
	holder->var_value = ft_itoa(shell->past_exit_status);
	if (!holder->var_value)
		return (0);
	if (!replace_var_with_value(holder, shell, 1))
		return (0);
	return (1);
}

char	*expand_variables(char *str, t_shell *shell)
{
	t_expand	holder;

	if (!str)
		return (NULL);
	
	holder.result = ft_strdup(str);
	if (!holder.result)
		return (NULL);
	
	holder.indx = 0;
	while (holder.result[holder.indx])
	{
		if (holder.result[holder.indx] == '$')
		{
			holder.var_start = &holder.result[holder.indx + 1];
			if (*holder.var_start == '?')
			{
				if (!handle_question_mark(&holder, shell))
					break;
				continue;
			}
			
			holder.var_end = holder.var_start;
			while (*holder.var_end && (ft_isalnum(*holder.var_end) || *holder.var_end == '_'))
				holder.var_end++;
			
			if (holder.var_end > holder.var_start)
			{
				holder.var_name = ft_substr(holder.var_start, 0, holder.var_end - holder.var_start);
				if (!holder.var_name)
					break;
				
				char *env_val = get_env_value(holder.var_name, shell);
				holder.var_value = env_val ? env_val : "";
				
				if (!replace_var_with_value(&holder, shell, 0))
					break;
				continue;
			}
		}
		holder.indx++;
	}
	
	return (holder.result);
}

char	*expand_variables_quoted(char *str, t_shell *shell)
{
	// For now, use the same expansion logic
	// In a full implementation, this would handle quote context
	return (expand_variables(str, shell));
}
