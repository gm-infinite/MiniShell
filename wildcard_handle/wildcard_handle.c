/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_handle.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 13:19:03 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/04/15 14:54:16 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../main/minishell.h"
#include <dirent.h>

char *wildcard_handle(char* str)
{
	DIR *current_dir;
	char hidden_flag;

	current_dir = opendir(".");
	if()
	struct dirent *entry;
	while ((entry = readdir(current_dir)) != NULL) {
    printf("%s\n", entry->d_name);
	}
	closedir(current_dir);
}

int main()
{
	char *print = wildcard_handle("");
	printf("%s", print);
	free(print);
}