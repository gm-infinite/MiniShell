/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_handler.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 11:00:28 by emgenc            #+#    #+#             */
/*   Updated: 2025/07/25 12:26:50 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WILDCARD_HANDLER_H
# define WILDCARD_HANDLER_H

# include "../../main/minishell.h"
# include <dirent.h>

void	apply_filter_minlen(t_split filter, t_split cur_dir,
			char *check_list, char *wildcard);
void	apply_filter_start(t_split filter, t_split cur_dir, char *check_list);
void	apply_filter_middle(t_split filter, t_split cur_dir, char *check_list);
void	apply_filter_end(t_split filter, t_split cur_dir, char *check_list);
void	apply_filter(t_split cur_dir, char *check_list, char *wildcard);
int		ft_strrcmp(const char *s1, const char *s2);
int		setup_filter_memory(t_split *filter, char *processed_wildcard);
int		handle_single_quote(const char *wildcard, int *i,
			int *in_single_quotes, int in_double_quotes);
int		handle_double_quote(const char *wildcard, int *i,
			int *in_double_quotes, int in_single_quotes);
char	*init_pattern_result(const char *wildcard, int *vars);
void	process_wildcard_loop(const char *wildcard, char *result, int *vars);

#endif