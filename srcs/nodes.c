/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 08:13:36 by bleow             #+#    #+#             */
/*   Updated: 2025/03/19 21:55:07 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates and links a new node in the token linked list.
- Creates node with current token type and data.
- Links node to existing list if available.
- Updates vars->current to point to the new node.
Returns:
- 1 on success.
- 0 on allocation failure.
Works with lexerlist() during token list creation.

Example: For token "echo"
- Creates TYPE_CMD node with "echo" as data
- Links to previous token if it exists
- Updates current token pointer to this new node
*/
int	makenode(t_vars *vars, char *data)
{
	t_node	*newnode;

	newnode = initnode(vars->curr_type, data);
	if (!newnode)
		return (0);
	if (vars->current)
	{
		vars->current->next = newnode;
		newnode->prev = vars->current;
	}
	vars->current = newnode;
	return (1);
}

/*
Adds a child node to a parent node in the AST.
- Follows left-to-right priority for child placement.
- Places in left child position if available.
- Otherwise places in right child position if available.
- Does nothing if both positions are already filled.
Works with build_ast() during AST construction.

Example: Adding redirection to command node
- First tries to place in left child slot
- If left is occupied, places in right child slot
- Updates child->prev to point to parent
*/
void	add_child(t_node *parent, t_node *child)
{
	if (!parent || !child)
		return ;
	if (!parent->left)
	{
		parent->left = child;
		child->prev = parent;
	}
	else if (!parent->right)
	{
		parent->right = child;
		child->prev = parent;
	}
}

/*
Handles pipe node insertion in AST construction.
- Creates a new pipe node as a junction point.
- Reorganizes tree to maintain pipeline order.
- Handles both initial pipe and subsequent pipes.
Works with build_ast() when processing pipe tokens.

Example: For "cmd1 | cmd2 | cmd3"
- First pipe creates root with cmd1 left, cmd2 right
- Second pipe creates new root with old root left, cmd3 right
- Maintains correct command execution order
*/
void handle_pipe_node(t_node **root, t_node *pipe_node)
{
	t_node *new_pipe;

	new_pipe = initnode(TYPE_PIPE, "|");
	if (!new_pipe)
		return ;
	if (!*root)
	{
		*root = new_pipe;
		(*root)->left = pipe_node->prev;
		(*root)->right = pipe_node->next;
	}
	else
	{
		new_pipe->left = *root;
		new_pipe->right = pipe_node->next;
		*root = new_pipe;
	}
}

/*
Attaches redirection node to appropriate command node.
- Finds relevant command by traversing right branch.
- Skips pipe nodes to find actual command node.
- Places redirection in command's right child.
- Preserves existing redirections if present.
Works with build_ast() when processing redirection tokens.

Example: For "cmd > file > file2"
- First redirection is attached directly to cmd's right
- Second redirection is attached with first redirection pushed down
- Creates a chain of redirections linked to the command
*/
void	redirection_node(t_node *root, t_node *redir_node)
{
	t_node	*cmd_node;

	cmd_node = root;
	while (cmd_node && cmd_node->type == TYPE_PIPE)
		cmd_node = cmd_node->right;
	if (cmd_node)
	{
		if (!cmd_node->right)
			cmd_node->right = redir_node;
		else
		{
			redir_node->left = cmd_node->right;
			cmd_node->right = redir_node;
		}
	}
}
