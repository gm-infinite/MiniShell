# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/13 12:36:09 by kuzyilma          #+#    #+#              #
#    Updated: 2025/07/20 22:15:05 by emgenc           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Source files organized by functionality
MAIN_SRC = main.c main_utils.c environment.c env_expand.c env_replace.c env_tilde.c get_next_line.c
BUILTIN_SRC = builtin_utils.c cd.c cd_utils.c echo.c env.c exit.c export.c export_utils.c export_sort.c export_process.c export_env.c export_append.c export_process_utils.c pwd.c unset.c unset_utils.c
EXECUTE_SRC = execute.c exec_utils.c signal.c exec_single.c exec_parentheses.c exec_dispatch.c pipe_management.c arg_utils.c path_utils.c exec_validation.c pipeline_utils.c exec_arg_utils.c exec_paren_utils.c
PIPES_SRC = pipes.c count_pipes.c split_by_pipes.c execute_pipe_command.c execute_pipe_child.c pipes_utils.c pipe_child_utils.c
REDIR_SRC = redirections.c heredoc.c redir_utils.c redir_syntax.c redirect_utils.c redir_exec_utils.c redir_external.c redir_args.c redir_syntax_utils.c redir_type_utils.c
PARSER_SRC = quotes.c quotes_utils.c
SPLIT_SRC = t_split_utils.c ft_split_quotes.c t_split_utils2.c
AOPSR_SRC = parser_and_or.c and_or_utils.c and_or_helpers.c parser_and_or2.c syntax.c parser_and_or_utils.c
SOA_SRC = sep_opt_arg.c
WILDCARD_SRC = wildcard_handle.c wildcard_filter.c wildcard_filter2.c wildcard_utils.c
GLOBAL_SRC = global.c

SRC = $(addprefix main/, $(MAIN_SRC)) \
      $(addprefix builtin_cmd/, $(BUILTIN_SRC)) \
      $(addprefix execute/, $(EXECUTE_SRC)) \
      $(addprefix execute/pipes/, $(PIPES_SRC)) \
      $(addprefix execute/redirections/, $(REDIR_SRC)) \
      $(addprefix parser/, $(PARSER_SRC)) \
      $(addprefix parser/t_split_utils/, $(SPLIT_SRC)) \
      $(addprefix parser/and_or_parser/, $(AOPSR_SRC)) \
      $(addprefix parser/sep_opt_arg/, $(SOA_SRC)) \
      $(addprefix parser/wildcard_handle/, $(WILDCARD_SRC)) \
      $(addprefix global/, $(GLOBAL_SRC))

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
