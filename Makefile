# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kuzyilma <kuzyilma@student.42istanbul.c    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/13 12:36:09 by kuzyilma          #+#    #+#              #
#    Updated: 2025/02/13 13:52:07 by kuzyilma         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAIN_SRC = main.c is_empty.c

SRC = $(addprefix main/, $(MAIN_SRC))

CC = cc
CFLAGS = -Wall -Werror -Wextra
LFLAGS = -lreadline
OSRC = $(SRC:.c=.o)

NAME = minishell

all: $(NAME)
	
$(NAME): $(OSRC)
	$(CC) $(CFLAGS) $(OSRC) $(LFLAGS) -o $(NAME)

clean:
	rm -rf $(OSRC)

fclean: clean
	rm rf $(NAME)

re: fclean all

main: all clean
	clear

.PHONY: all clean fclean re main
