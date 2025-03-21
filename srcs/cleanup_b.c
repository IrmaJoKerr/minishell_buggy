/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_b.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:50 by bleow             #+#    #+#             */
/*   Updated: 2025/03/21 11:45:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Recursively free the abstract syntax tree (AST) nodes.
- Traverses the tree in post-order (left, right, current).
- Frees all argument arrays associated with nodes.
- Ensures complete cleanup of complex command structures.
Works with cleanup_vars() and cleanup_exit().

Example: For "ls -la | grep .c > output.txt":
- Recursively frees all nodes including command, pipe, and redirection
- Frees all argument arrays like ["ls", "-la"] and ["grep", ".c"]
OLD VERSION PRE DEBUG
void	cleanup_ast(t_node *node)
{
    if (!node)
        return ;
    cleanup_ast(node->left);
    cleanup_ast(node->right);
    if (node->args)
        ft_free_2d(node->args, ft_arrlen(node->args));
    ft_safefree((void **)&node);
}
*/
void	cleanup_ast(t_node *node)
{
    static int depth = 0;
    int my_depth;
    
    if (!node)
        return;
    
    depth++;
    my_depth = depth;
    fprintf(stderr, "DEBUG: [cleanup_ast] Cleaning node at depth %d, type %d\n", 
            my_depth, node->type);
    
    cleanup_ast(node->left);
    cleanup_ast(node->right);
    
    if (node->args)
    {
        fprintf(stderr, "DEBUG: [cleanup_ast] Freeing args for node at depth %d\n", my_depth);
        ft_free_2d(node->args, ft_arrlen(node->args));
    }
    
    ft_safefree((void **)&node);
    fprintf(stderr, "DEBUG: [cleanup_ast] Node at depth %d freed\n", my_depth);
    depth--;
}

/*
Free a single token node and its arguments.
- Takes a node pointer and frees its arguments array.
- Then frees the node itself.
- Used for individual node cleanup without recursion.
Works with cleanup_token_list().
OLD VERSION PRE DEBUG
void	free_token_node(t_node *node)
{
    int	i;
    
    if (!node)
        return ;
    if (node->args)
    {
        i = 0;
        while (node->args[i])
        {
            ft_safefree((void **)&node->args[i]);
            i++;
        }
        ft_safefree((void **)&node->args);
    }
    ft_safefree((void **)&node);
}
*/
void	free_token_node(t_node *node)
{
    int	i;
    
	fprintf(stderr, "DEBUG: [free_token_node] Starting to free node %p of type %d\n", 
		(void*)node, node->type);
    if (!node)
    {
        fprintf(stderr, "DEBUG: [free_token_node] Node is NULL\n");
        return;
    }
    
    fprintf(stderr, "DEBUG: [free_token_node] Freeing node type %d\n", node->type);
    
    if (node->args)
    {
        i = 0;
        while (node->args[i])
        {
            fprintf(stderr, "DEBUG: [free_token_node] Freeing arg[%d]: '%s'\n", 
                    i, node->args[i]);
            ft_safefree((void **)&node->args[i]);
            i++;
        }
        ft_safefree((void **)&node->args);
        fprintf(stderr, "DEBUG: [free_token_node] Freed %d args\n", i);
    }
    
    ft_safefree((void **)&node);
    fprintf(stderr, "DEBUG: [free_token_node] Node freed\n");
}

/*
Clean up the token list by freeing all nodes.
- Traverses the linked list of tokens.
- Frees each node and its arguments.
- Resets head and current pointers in vars.
- Called when processing a new command line.
Works with lexerlist() and cleanup_exit().
OLD VERSION PRE DEBUG
void	cleanup_token_list(t_vars *vars)
{
    t_node	*current;
    t_node	*next;
    
    if (!vars || !vars->head)
        return ;
    current = vars->head;
    while (current)
    {
        next = current->next;
        free_token_node(current);
        current = next;
    }
    vars->head = NULL;
    vars->current = NULL;
    vars->astroot = NULL;
}
*/
/*NEWER OLD VERSION
void	cleanup_token_list(t_vars *vars)
{
    t_node	*current;
    t_node	*next;
    int     count;
    
    if (!vars || !vars->head)
    {
        fprintf(stderr, "DEBUG: [cleanup_token_list] No tokens to clean up\n");
        return;
    }
    fprintf(stderr, "DEBUG: [cleanup_token_list] Starting token list cleanup\n");
    current = vars->head;
    count = 0;
    while (current)
    {
        next = current->next;
        fprintf(stderr, "DEBUG: [cleanup_token_list] Freeing token %d\n", count++);
        free_token_node(current);
        current = next;
    }
    vars->head = NULL;
    vars->current = NULL;
    vars->astroot = NULL;
    fprintf(stderr, "DEBUG: [cleanup_token_list] %d tokens cleaned up\n", count);
}
*/
void cleanup_token_list(t_vars *vars)
{
    t_node *current;
    t_node *next;
    int count;

    fprintf(stderr, "DEBUG: [cleanup_token_list] Starting token list cleanup\n");
    
    if (!vars || !vars->head)
    {
        fprintf(stderr, "DEBUG: [cleanup_token_list] Nothing to clean up (head=%p)\n", 
                vars ? (void*)vars->head : NULL);
        return;
    }

    fprintf(stderr, "DEBUG: [cleanup_token_list] Starting cleanup with head=%p\n",
            (void*)vars->head);

    current = vars->head;
    count = 0;
    while (current)
    {
        next = current->next;
        fprintf(stderr, "DEBUG: [cleanup_token_list] Freeing token %d\n", count);
        free_token_node(current);
        current = next;
        count++;
    }

    vars->head = NULL;
    vars->current = NULL;
    fprintf(stderr, "DEBUG: [cleanup_token_list] %d tokens cleaned up\n", count);
    fprintf(stderr, "DEBUG: [cleanup_token_list] Completed cleanup, head now=%p\n", 
            (void*)vars->head);
}

/*
Clean up execution context.
- Frees command path string.
- Frees the exec structure itself.
- Prevents memory leaks after command execution.
Works with init_exec_context().
*/
void	cleanup_exec_context(t_exec *exec)
{
    if (!exec)
        return ;
    if (exec->cmd_path)
        ft_safefree((void **)&exec->cmd_path);
    ft_safefree((void **)&exec);
}

/*
Clean up file descriptors after command execution.
- Safely closes input and output file descriptors.
- Avoids closing standard descriptors (0, 1, 2).
- Prevents descriptor leaks after redirections.
Works with execute_cmd() and handle_redirect_cmd().
*/
void	cleanup_fds(int fd_in, int fd_out)
{
    if (fd_in > 2 && fd_in != STDIN_FILENO)
        close(fd_in);
    if (fd_out > 2 && fd_out != STDOUT_FILENO)
        close(fd_out);
}
