# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/23 18:10:33 by bleow             #+#    #+#              #
#    Updated: 2024/02/23 18:10:41 by bleow            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libftprintf.a

FLAGS = -Wall -Wextra -Werror

SRCS = 	arg_c.c \
		arg_di.c \
		arg_p.c \
		arg_s.c \
		arg_u.c \
		arg_x_lower.c \
		arg_x_upper.c \
		master_parser.c \
		ft_printf.c \

LIBFT = ./libft/

LIBFT_A = ./libft/libft.a

OBJS = $(SRCS:.c=.o)

%o: %.c
	gcc -c $(FLAGS) -g -c $< -o $(<:.c=.o)

$(NAME): $(OBJS)
	make -C $(LIBFT)
	cp $(LIBFT_A) $(NAME)
	ar -rcs $(NAME) $(OBJS)
	
all: $(NAME)

clean:
	make clean -C $(LIBFT)
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME) $(LIBFT_A)

re: fclean all

.PHONY: all, clean, fclean, re