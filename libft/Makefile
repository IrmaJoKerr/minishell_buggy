# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/23 18:10:33 by bleow             #+#    #+#              #
#    Updated: 2025/03/13 09:54:05 by bleow            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

INCLUDE_DIRS = $(shell find . -type d) includes
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

CFLAGS = -Wall -Wextra -Werror -std=c89 $(INCLUDES) -c
DEBUG_FLAGS = -gdwarf-4
SANITIZE_FLAGS = -fsanitize=address

CFILES = \
	lib_ft/ft_arrlen.c \
	lib_ft/ft_atoi.c \
	lib_ft/ft_atoiarr_cln.c \
	lib_ft/ft_atoiarr_errcln.c \
	lib_ft/ft_atoierr.c \
	lib_ft/ft_bzero.c \
	lib_ft/ft_calloc.c \
	lib_ft/ft_free_2d.c \
	lib_ft/ft_isalnum.c \
	lib_ft/ft_isalpha.c \
	lib_ft/ft_isascii.c \
	lib_ft/ft_isdelimiter_quote.c \
	lib_ft/ft_isdelimiter.c \
	lib_ft/ft_isdigit.c \
	lib_ft/ft_isprint.c \
	lib_ft/ft_isquote.c \
	lib_ft/ft_isspace.c \
	lib_ft/ft_itoa.c \
	lib_ft/ft_memchr.c \
	lib_ft/ft_memcmp.c \
	lib_ft/ft_memcpy.c \
	lib_ft/ft_memmove.c \
	lib_ft/ft_memset.c \
	lib_ft/ft_putchar_fd.c \
	lib_ft/ft_putchar.c \
	lib_ft/ft_putendl_fd.c \
	lib_ft/ft_putendl.c \
	lib_ft/ft_putnbr_fd.c \
	lib_ft/ft_putnbr.c \
	lib_ft/ft_putstr_fd.c \
	lib_ft/ft_putstr.c \
	lib_ft/ft_safefree.c \
	lib_ft/ft_split.c \
	lib_ft/ft_splitstr.c \
	lib_ft/ft_strchr.c \
	lib_ft/ft_strcmp.c \
	lib_ft/ft_strdup.c \
	lib_ft/ft_striteri.c \
	lib_ft/ft_strjoin.c \
	lib_ft/ft_strlcat.c \
	lib_ft/ft_strlcpy.c \
	lib_ft/ft_strlen.c \
	lib_ft/ft_strmapi.c \
	lib_ft/ft_strncmp.c \
	lib_ft/ft_strndup.c \
	lib_ft/ft_strnlen.c \
	lib_ft/ft_strnstr.c \
	lib_ft/ft_strrchr.c \
	lib_ft/ft_strtrim.c \
	lib_ft/ft_substr.c \
	lib_ft/ft_tolower.c \
	lib_ft/ft_toupper.c \
	lib_ft/ft_lstadd_back.c \
	lib_ft/ft_lstadd_front.c \
	lib_ft/ft_lstclear.c \
	lib_ft/ft_lstdelone.c \
	lib_ft/ft_lstiter.c \
	lib_ft/ft_lstlast.c \
	lib_ft/ft_lstmap.c \
	lib_ft/ft_lstnew.c \
	lib_ft/ft_lstsize.c 

GNLFILES = \
	get_next_line/get_next_line.c \
	get_next_line/get_next_line_utils.c
	
PRINTF_FILES = \
	libftprintf/ft_printf.c \
	libftprintf/master_parser.c \
	libftprintf/arg_c.c \
	libftprintf/arg_s.c \
	libftprintf/arg_di.c \
	libftprintf/arg_p.c \
	libftprintf/arg_u.c \
	libftprintf/arg_x_lower.c \
	libftprintf/arg_x_upper.c

OFILES = $(CFILES:%.c=objects/%.o)
GNLOBJS = $(GNLFILES:%.c=objects/%.o)
PRINTF_OBJS = $(PRINTF_FILES:%.c=objects/%.o)

all: $(NAME)

$(NAME): $(OFILES) $(GNLOBJS) $(PRINTF_OBJS)
	ar rcs $(NAME) $(OFILES) $(GNLOBJS) $(PRINTF_OBJS)  
	ranlib $(NAME)

objects/%.o: %.c
	@mkdir -p $(dir $@)
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -rf objects

fclean: clean
	rm -f $(NAME)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: re

sanitize: CFLAGS += $(SANITIZE_FLAGS)
sanitize: re

valgrind: debug
	valgrind --leak-check=full --track-origins=yes ./$(NAME)

re: fclean all

.PHONY: all clean fclean re debug sanitize valgrind