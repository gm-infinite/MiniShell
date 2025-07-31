# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/25 10:52:25 by emgenc            #+#    #+#              #
#    Updated: 2025/07/31 16:21:43 by emgenc           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAIN_SRC = main.c main_utils.c
ENV_SRC = environment.c replacevar.c variable_expander.c

BUILTIN_SRC = builtin_utils.c env_and_echo.c exit.c
UNSET_SRC = unset.c unset_utils.c
CD_SRC = cd.c cd_utils.c
EXPORT_SRC = export.c export_utils.c export_env.c export_append.c export_process_utils.c export_process_utils_2.c

EXECUTE_SRC = execute.c signal.c validate_executable.c
EXEC_BUILTINS_SRC = exec_builtin_with_redirs.c
EXEC_SYNTAX_VALIDATION_SRC = excess_parantheses_remover.c main_syntax_validation.c
PIPES_SRC = create_pipes.c external_commands.c pipe_clean.c pipe_executor.c pipes_only.c process_expanded_args.c parentheses_removal.c
PIPES_WITH_REDIRS_SRC = getters.c p_w_r_execution_engine.c pipes_with_redirs.c utils_for_p_w_r.c
REDIR_SRC = redirs_only.c exec_extern_with_redirs.c
SIMPLE_CMD_SRC = simple_command_executor.c

EXEC_UTILS_SRC = arg_expander_loop.c check_if_path_is_dir.c cleanup.c dollar_expander.c expandvar_quoted.c pipe_utils.c split_by_pipes.c split_to_args.c tilde_handler.c
EXECUTABLE_FINDER_SRC = executable_finder.c utils_for_finder.c
HEREDOC_REGULAR_SRC = child_handler.c heredoc.c quote_remover_for_redir.c
PROCESS_ARGS_QUOTES_SRC = process_args_quotes.c process_args_quotes_utils.c
REDIR_PARSER_SRC = 1_parse_redirections.c clean_heredoc.c 2_processer_loop.c handle_expandable_part.c quote_expander.c redirection_type_handler.c
SINGLE_COMMAND_SRC = execute_single_command.c handlers_for_single.c

SPLIT_SRC = t_split_utils.c ft_split_quotes.c t_split_utils2.c
AOPSR_SRC = parser_and_or.c parser_and_or_process_utils.c parser_and_or_parentheses_utils.c and_or_utils.c parser_and_or2.c syntax.c parser_and_or_utils.c
SOA_SRC = sep_opt_arg.c
WILDCARD_SRC = wildcard_handle.c wildcard_filter.c wildcard_filter2.c wildcard_filter2_utils.c wildcard_quote_utils.c wildcard_utils.c

SRC = $(addprefix main/, $(MAIN_SRC)) \
      $(addprefix main/environment/, $(ENV_SRC)) \
      $(addprefix builtin_cmd/, $(BUILTIN_SRC)) \
      $(addprefix builtin_cmd/export/, $(EXPORT_SRC)) \
      $(addprefix builtin_cmd/cd/, $(CD_SRC)) \
      $(addprefix builtin_cmd/unset/, $(UNSET_SRC)) \
      $(addprefix execute/, $(EXECUTE_SRC)) \
      $(addprefix execute/builtins_with_redirects/, $(EXEC_BUILTINS_SRC)) \
      $(addprefix execute/main_syntax_validation/, $(EXEC_SYNTAX_VALIDATION_SRC)) \
      $(addprefix execute/pipes/, $(PIPES_SRC)) \
      $(addprefix execute/pipes_with_redirections/, $(PIPES_WITH_REDIRS_SRC)) \
      $(addprefix execute/redirections/, $(REDIR_SRC)) \
      $(addprefix execute/simple_command/, $(SIMPLE_CMD_SRC)) \
      $(addprefix execute/utils/, $(EXEC_UTILS_SRC)) \
      $(addprefix execute/utils/executable_finder/, $(EXECUTABLE_FINDER_SRC)) \
      $(addprefix execute/utils/heredoc_for_pipes/, $(HEREDOC_FOR_PIPES_SRC)) \
      $(addprefix execute/utils/heredoc_regular/, $(HEREDOC_REGULAR_SRC)) \
      $(addprefix execute/utils/process_args_quotes/, $(PROCESS_ARGS_QUOTES_SRC)) \
      $(addprefix execute/utils/redirection_parser/, $(REDIR_PARSER_SRC)) \
      $(addprefix execute/utils/single_command/, $(SINGLE_COMMAND_SRC)) \
      $(addprefix parser/t_split_utils/, $(SPLIT_SRC)) \
      $(addprefix parser/and_or_parser/, $(AOPSR_SRC)) \
      $(addprefix parser/sep_opt_arg/, $(SOA_SRC)) \
      $(addprefix parser/wildcard_handle/, $(WILDCARD_SRC)) \

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
	rm -f $(NAME) minishell_bonus

re: fclean all

main: all
	make clean -C ./e-libft
	rm -f $(OSRC) 
	clear


.PHONY: all clean fclean re main bonus
