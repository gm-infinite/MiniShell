/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_command.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:45:41 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 17:35:07 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../main/minishell.h"

static int	validate_and_process_args(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	if (!args || !args[0])
		return (1);
	i = 0;
	while (args[i])
	{
		expanded = expandvar_quoted(args[i], shell);
		if (expanded && expanded != args[i])
		{
			free(args[i]);
			args[i] = expanded;
		}
		i++;
	}
	process_args_quotes(args, shell);
	if (!args[0])
		return (0);
	if (args[0][0] == '\0')
	{
		write(STDERR_FILENO, ": command not found\n", 20);
		return (127);
	}
	return (-1);
}

static int	create_and_execute_process(char **args, char *executable,
		t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(executable);
		return (1);
	}
	else if (pid == 0)
		return (exec_child_process(args, executable, shell));
	else
		return (handle_parent_process(pid, executable));
}

static int	execute_external_command(char **args, t_shell *shell)
{
	char	*executable;
	int		validation_result;

	executable = find_executable(args[0], shell);
	validation_result = validate_exec(args, executable);
	if (validation_result != 0)
	{
		if (executable && validation_result != 126)
			free(executable);
		return (validation_result);
	}
	return (create_and_execute_process(args, executable, shell));
}

int	execute_single_command(char **args, t_shell *shell)
{
	int	validation_result;

	validation_result = validate_and_process_args(args, shell);
	if (validation_result != -1)
		return (validation_result);
	if (is_builtin(args[0]))
		return (execute_builtin(args, shell));
	return (execute_external_command(args, shell));
}
