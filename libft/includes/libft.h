/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:10:58 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 03:27:37 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stddef.h>
# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include <wchar.h>
# include <stdint.h>
# include <limits.h>
# include "get_next_line.h"
# include "ft_printf.h"

# ifndef LLONG_MAX
#  define LLONG_MAX 9223372036854775807LL
# endif

# ifndef LLONG_MIN
#  define LLONG_MIN 9223372036854775807LL
# endif

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

size_t		ft_arrlen(char **array);
int			ft_atoi(const char *str);
int			*ft_atoiarr_cln(char **temparr, int len);
int			*ft_atoiarr_errcln(char **temparr, int len);
long long	ft_atoierr(const char *str);
void		ft_bzero(void *s, size_t n);
void		*ft_calloc(size_t num, size_t size);
void		ft_free_2d(char **splits, size_t i);
int			ft_isalnum(int c);
int			ft_isalpha(int c);
int			ft_isascii(int c);
int			ft_isdelimiter_quote(char c, char *delimiters, int *in_quotes,
				char *quote_char);
int			ft_isdelimiter(char c, char *delimiters);
int			ft_isdigit(int c);
int			ft_isprint(int c);
int			ft_isquote(char c);
int			ft_isspace(int c);
char		*ft_itoa(int n);
void		*ft_memchr(const void *str, int c, size_t n);
int			ft_memcmp(const void *s1, const void *s2, size_t n);
void		*ft_memcpy(void *dest, const void *src, size_t n);
void		*ft_memmove(void *dest, const void *src, size_t n);
void		*ft_memset(void *dest, int c, size_t count);
void		ft_putchar_fd(char c, int fd);
void		ft_putchar(char c);
void		ft_putendl_fd(char *s, int fd);
void		ft_putendl(char *s);
void		ft_putnbr_fd(int n, int fd);
void		ft_putnbr(int n);
void		ft_putstr_fd(char *s, int fd);
void		ft_putstr(char *s);
void		ft_safefree(void **ptr);
char		**ft_split(char const *s, char c);
char		**ft_splitstr(char const *s, char *delimiters);
char		*ft_strchr(const char *str, int character);
int			ft_strcmp(const char *str1, const char *str2);
char		*ft_strdup(const char *str);
void		ft_striteri(char *s, void (*f)(unsigned int, char*));
char		*ft_strjoin(const char *s1, const char *s2);
size_t		ft_strlcat(char *dest, const char *src, size_t size);
size_t		ft_strlcpy(char *dest, const char *src, size_t size);
size_t		ft_strlen(const char *str);
char		*ft_strmapi(char const *s, char (*f)(unsigned int, char));
int			ft_strncmp(const char *str1, const char *str2, size_t n);
char		*ft_strndup(const char *s, size_t n);
size_t		ft_strnlen(const char *s, size_t maxlen);
char		*ft_strnstr(const char *pile, const char *find, size_t len);
char		*ft_strrchr(const char *str, int character);
char		*ft_strtrim(char const *s1, char const *set);
char		*ft_substr(char const *s, unsigned int start, size_t len);
int			ft_tolower(int c);
int			ft_toupper(int c);
t_list		*ft_lstnew(void *content);
void		ft_lstadd_front(t_list **lst, t_list *new);
int			ft_lstsize(t_list *lst);
t_list		*ft_lstlast(t_list *lst);
void		ft_lstadd_back(t_list **lst, t_list *new);
void		ft_lstdelone(t_list *lst, void (*del)(void *));
void		ft_lstclear(t_list **lst, void (*del)(void *));
void		ft_lstiter(t_list *lst, void (*f)(void *));
t_list		*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));

#endif