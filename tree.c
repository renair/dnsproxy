#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#include "tree.h"

void dump_tree_recurse(FILE*, tree*);

tree* create_node(const char* addr)
{
	tree* node = (tree*)malloc(sizeof(tree));
	node->_left = 0;
	node->_right = 0;
	unsigned int len = strlen(addr);
	node->_addr = (char*)malloc(len);
	strncpy(node->_addr, addr, len);
	return node;
}

void clean_tree(tree* root)
{
	if(root != 0)
	{
		clean_tree(root->_left);
		clean_tree(root->_right);
		free(root->_addr);
		free(root);
	}
}

void tree_add(tree* t, char* str)
{
	if(strcmp(t->_addr, str) < 0)
	{
		if(t->_left == 0)
		{
			t->_left = create_node(str);
			return;
		}
		else
		{
			tree_add(t->_left, str);
		}
	}
	else if(strcmp(t->_addr, str) > 0)
	{
		if(t->_right == 0)
		{
			t->_right = create_node(str);
			return;
		}
		else
		{
			tree_add(t->_right, str);
		}
	}
}

int is_exist(tree* t, char* addr)
{
	if(strcmp(t->_addr, addr) < 0)
	{
		return t->_left == 0 ? 0 : is_exist(t->_left, addr);
	}
	else if(strcmp(t->_addr, addr) > 0)
	{
		return t->_right == 0 ? 0 : is_exist(t->_right, addr);
	}
	else
	{
		return 1;
	}
}

void print_tree(tree* node)
{
	if(node != 0)
	{
//		char ip[15];
		printf("%s\n",node->_addr);
		print_tree(node->_left);
		print_tree(node->_right);
	}
}
