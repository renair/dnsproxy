#ifndef TREE_H
#define TREE_H

#include<string.h>

struct tree
{
	struct tree* _left;
	struct tree* _right;
	unsigned char* _addr;
};

typedef struct tree tree;

tree* create_node(const char*);
void clean_tree(tree*);
void tree_add(tree*, char*);
int is_exist(tree*, char*);
void print_tree(tree*);

#endif
