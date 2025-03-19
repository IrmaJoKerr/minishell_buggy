/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 10:27:35 by bleow             #+#    #+#             */
/*   Updated: 2024/09/25 00:13:29 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef MAX_FD
#  define MAX_FD 1024
# endif 

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1000
# endif

# include "libft.h"
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stddef.h>
# include <limits.h>

char	*get_next_line(int fd);
char	*ft_read_fd(int fd, char *last_pos);
char	*ft_newline(char *last_pos);
char	*ft_trim(char *last_pos);
char	*ft_gstrjoin(char *s1, char *s2);

#endif