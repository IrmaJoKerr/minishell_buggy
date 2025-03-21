/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_safefree.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 01:08:16 by bleow             #+#    #+#             */
/*   Updated: 2025/03/20 23:30:15 by bleow            ###   ########.fr       */
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
/*
void	ft_safefree(void **ptr)
{
    fprintf(stderr, "DEBUG: [ft_safefree] Called with ptr=%p, *ptr=%p\n", 
            (void*)ptr, ptr ? *ptr : NULL);
            
    if (ptr && *ptr)
    {
        free(*ptr);
        *ptr = NULL;
        fprintf(stderr, "DEBUG: [ft_safefree] Freed memory and set *ptr=NULL\n");
    }
    else
    {
        fprintf(stderr, "DEBUG: [ft_safefree] Nothing to free (NULL pointer)\n");
    }
}
*/