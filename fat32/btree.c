#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

btree *add_node(btree *p, file_t file)
{
    if (p == NULL)
    {
        p = malloc(sizeof(btree));
        p->file = file;
        p->left = NULL;
        p->right = NULL;
    }
    else if (strcmp(file.short_file_name, p->file.short_file_name) < 0)
        p->left = add_node(p->left, file);
    else if (strcmp(file.short_file_name, p->file.short_file_name) > 0)
        p->right = add_node(p->right, file);
    return p;
}

void delete_tree(btree *p)
{
    btree *left;
    btree *right;

    if (p == NULL) return;
    left = p->left;
    right = p->right;

    delete_tree(left);
    free(p);
    delete_tree(right);
}

void print_tree(btree *p)
{
    if (p == NULL) return;
    print_tree(p->left);
    fat_ls_file(&(p->file));
    print_tree(p->right);
}
