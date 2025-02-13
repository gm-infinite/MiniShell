/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:55:46 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/02/13 14:26:00 by kuzyilma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//safe exit for ctrl + D 
//(might be used later for other stuff)
void safe_exit(t_shell *shell)
{
    if (shell->current_input != NULL)
        free(shell->current_input);
    rl_clear_history();
    exit(0);
}

//initiates shell struct, signal's and start of readline sequence
static void start_shell(t_shell *shell)
{
    //if needed initiate starting values for shell
    //we need to handle signals at some point
    while(1)
    {
        shell->current_input = readline("minishell > ");
        if(shell->current_input == NULL)
            safe_exit(shell);
        if (!is_empty(shell->current_input))
            add_history(shell->current_input);
        //parsing starts here
        free(shell->current_input);
    }
}

int main()
{
    t_shell shell;

    start_shell(&shell);
}
