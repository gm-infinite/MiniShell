# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/13 12:36:09 by kuzyilma          #+#    #+#              #
#    Updated: 2025/06/02 15:09:12 by kuzyilma         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAIN_SRC = main.c main_utils.c
SPLIT_SRC = t_split_utils.c ft_split_quotes.c
AOPSR_SRC = parser_and_or.c and_or_utils.c parser_and_or2.c
SOA_SRC = sep_opt_arg/sep_opt_arg.c

SRC = $(addprefix main/, $(MAIN_SRC)) $(addprefix t_split_utils/, $(SPLIT_SRC)) $(addprefix and_or_parser/, $(AOPSR_SRC)) $(SOA_SRC)

CC = cc
CFLAGS = -Wall -Werror -Wextra
LIBFT = e-libft/libft.a
LFLAGS = -lreadline
OSRC = $(SRC:.c=.o)

NAME = minishell

all: $(NAME)

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
