# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/13 12:36:09 by kuzyilma          #+#    #+#              #
#    Updated: 2025/07/24 18:57:43 by emgenc           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Source files organized by functionality
MAIN_SRC = main.c main_utils.c environment.c env_expand.c env_expand_utils.c env_quote_utils.c env_expansion_utils.c env_replace.c env_tilde.c
BUILTIN_SRC = builtin_utils.c cd.c cd_utils.c echo.c env.c exit.c export.c export_utils.c export_env.c export_append.c export_process_utils.c pwd.c unset.c unset_utils.c export_process_utils_2.c
EXECUTE_SRC = execute.c exec_utils.c exec_args_utils.c signal.c exec_single.c exec_parentheses.c exec_dispatch.c exec_arg_process_utils.c pipe_management.c arg_utils.c path_utils.c path_search_utils.c exec_validation.c pipeline_utils.c heredoc_utils.c pipeline_process_utils.c pipeline_child_utils.c exec_arg_utils.c exec_wildcard_utils.c execute_redirection_utils.c child_cleanup.c heredoc_utils_2.c
PIPES_SRC = pipes.c split_by_pipes.c execute_pipe_command.c execute_pipe_child.c execute_pipe_child_utils.c pipes_utils.c pipe_child_utils.c execute_pipe_command_utils.c
REDIR_SRC = redirections.c heredoc.c heredoc_utils.c heredoc_input_utils.c redir_utils.c redir_syntax.c redirect_utils.c redir_quote_utils.c redir_process_utils.c redir_segment_utils.c redir_char_utils.c redir_exec_utils.c redir_external.c redir_args.c redir_syntax_utils.c redir_type_utils.c
PARSER_SRC = quotes.c quotes_utils.c quotes_processing_utils.c
SPLIT_SRC = t_split_utils.c ft_split_quotes.c t_split_utils2.c
AOPSR_SRC = parser_and_or.c parser_and_or_process_utils.c parser_and_or_parentheses_utils.c and_or_utils.c parser_and_or2.c syntax.c parser_and_or_utils.c
SOA_SRC = sep_opt_arg.c
WILDCARD_SRC = wildcard_handle.c wildcard_filter.c wildcard_filter2.c wildcard_filter2_utils.c wildcard_quote_utils.c wildcard_utils.c

SRC = $(addprefix main/, $(MAIN_SRC)) \
      $(addprefix builtin_cmd/, $(BUILTIN_SRC)) \
      $(addprefix execute/, $(EXECUTE_SRC)) \
      $(addprefix execute/pipes/, $(PIPES_SRC)) \
      $(addprefix execute/redirections/, $(REDIR_SRC)) \
      $(addprefix parser/, $(PARSER_SRC)) \
      $(addprefix parser/t_split_utils/, $(SPLIT_SRC)) \
      $(addprefix parser/and_or_parser/, $(AOPSR_SRC)) \
      $(addprefix parser/sep_opt_arg/, $(SOA_SRC)) \
      $(addprefix parser/wildcard_handle/, $(WILDCARD_SRC))

CC = cc
CFLAGS = -Wall -Werror -Wextra
LIBFT = e-libft/libft.a
LFLAGS = -lreadline
OSRC = $(SRC:.c=.o)

NAME = minishell

all: $(NAME)

bonus: $(OSRC) $(LIBFT)
	$(CC) $(CFLAGS) $(OSRC) $(LIBFT) $(LFLAGS) -o minishell_bonus

$(LIBFT):
	make all -C e-libft/

$(NAME): $(OSRC) $(LIBFT)
	$(CC) $(CFLAGS) $(OSRC) $(LIBFT) $(LFLAGS) -o $(NAME)

clean:
	make clean -C ./e-libft
	rm -f $(OSRC)

fclean: clean
	make fclean -C ./e-libft
	rm -f $(NAME) 

re: fclean all

main: all
	make clean -C ./e-libft
	rm -f $(OSRC) 
	clear


.PHONY: all clean fclean re main
