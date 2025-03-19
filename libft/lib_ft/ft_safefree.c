/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_safefree.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 01:08:16 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 12:15:35 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
ft_safefree performs:
- Null Pointer Check: It checks if both the pointer-to-pointer AND the actual
pointer are not NULL before freeing.
- Sets Pointer to NULL: After freeing, it sets the pointer to NULL, preventing
use-after-free bugs.
- Double-free Prevention: If `safe_free()` is accidentally called  twice on 
the same pointer, it won't cause a double-free error.
- Consistent Interface: Provides a standardized way to free memory 
throughout code.
*/

#include "libft.h"

void	ft_safefree(void **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}
