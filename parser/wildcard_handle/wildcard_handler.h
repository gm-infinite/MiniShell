/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_handler.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 13:53:36 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WILDCARD_HANDLER_H
# define WILDCARD_HANDLER_H

# include "../../main/minishell.h"
# include <dirent.h>

void	apply_filter_minlen(t_split filter, t_split cur_dir, \
							char *check_list, char *wildcard);
void	apply_filter_start(t_split filter, t_split cur_dir, char *check_list);
void	apply_filter_middle(t_split filter, t_split cur_dir, char *check_list);
void	apply_filter_end(t_split filter, t_split cur_dir, char *check_list);
void	apply_filter(t_split cur_dir, char *check_list, char *wildcard, t_shell *shell);
char	*wildcard_input_modify(char *current_input, t_shell *shell);
int		ft_strrcmp(const char *s1, const char *s2);
char	*ft_strjoin_sq_f(char **to_free, const char *to_add);

#endif