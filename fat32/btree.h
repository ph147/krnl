#ifndef _BTREE_H
#define _BTREE_H

#include "fat.h"

typedef struct btree_struct
{
    file_t file;
    struct btree_struct *left;
    struct btree_struct *right;
} btree;

btree *add_node(btree *p, file_t file);
void print_tree(btree *p);
void delete_tree(btree *p);

#endif
