/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdelimiter_quote.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 09:33:15 by bleow             #+#    #+#             */
/*   Updated: 2025/03/13 09:50:40 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_isdelimiter_quote() function is an enhanced version of ft_isdelimiter().
Ft_isdelimiter_quote() is able to handle quoted strings, where delimiters are
inside quotes. It will treat the quoted strings as a single string.

The function loops through the char *delimiters string, checking if any of the
characters in the string matches the character in the string ’delimiters’. 
If a match occurs for any of the characters in the delimiter string, the
function will consider c as a delimiter.

int *in_quotes is a pointer to an integer that keeps track of whether the
function is currently inside a quoted string. It persists between function
calls. Needed since quotes can be nested.

char *quote_char is a pointer to a char that keeps track of the type of quote
that the function is currently inside. Needed since quotes can be nested.
When calling the function for the first time, in_quotes and quote_char should
be initialized to 0 and '\0' respectively.

Example:
int in_quotes = 0;
char quote_char = '\0';

The function returns a 1 if c is a delimiter character.
Else it returns a 0.
*/
#include "libft.h"

int	ft_isdelimiter_quote(char c, char *delimiters, int *in_quotes,
	char *quote_char)
{
	if (!in_quotes || !quote_char)
		return (ft_isdelimiter(c, delimiters));
	if ((c == '"' || c == '\'') && !(*in_quotes))
	{
		*in_quotes = 1;
		*quote_char = c;
		return (0);
	}
	else if (*in_quotes && c == *quote_char)
	{
		*in_quotes = 0;
		return (0);
	}
	if (*in_quotes)
		return (0);
	return (ft_isdelimiter(c, delimiters));
}
