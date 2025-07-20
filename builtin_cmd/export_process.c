/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_process.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:00:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/20 22:30:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"

int	process_export_args(char **args, t_shell *shell)
{
	int		i;
	int		exit_status;
	char	*eq_pos;

	i = 1;
	exit_status = 0;
	while (args[i])
	{
		if (args[i][0] == '-')
		{
			print_option_error(args[i], shell);
			return (2);
		}
		eq_pos = ft_strchr(args[i], '=');
		if (eq_pos && eq_pos > args[i] && *(eq_pos - 1) == '+')
			exit_status |= process_append_export(args[i], eq_pos, shell);
		else if (eq_pos)
			exit_status |= process_assign_export(args[i], eq_pos, shell);
		else
			exit_status |= process_single_export(args[i], shell);
		i++;
	}
	shell->past_exit_status = exit_status;
	return (exit_status);
}
