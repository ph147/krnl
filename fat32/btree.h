#ifndef _BTREE_H
#define _BTREE_H

//#include "fat.h"

typedef struct btree_struct
{
    //file_t *file;
    char *value;
    struct btree_struct *left;
    struct btree_struct *right;
} btree;

btree *add_node(btree *p, char *value);
void print_tree(btree *p);

#endif
