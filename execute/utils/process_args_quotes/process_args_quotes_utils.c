/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_args_quotes_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:38:00 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/30 16:38:08 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	handle_escaped_quote(char *str, char *result, int i, int j)
{
	if (str[i] == '\001' && str[i + 1] == '\'')
	{
		result[j] = '\'';
		return (1);
	}
	return (0);
}

void	update_quote_states(char c, int *states)
{
	if (c == '\'' && !states[1])
		states[0] = !states[0];
	else if (c == '"' && !states[0])
		states[1] = !states[1];
}

int	should_copy_character(char c, int *states)
{
	if (c == '\'' && !states[1])
		return (0);
	if (c == '"' && !states[0])
		return (0);
	return (1);
}
