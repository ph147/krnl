#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

btree *add_node(btree *p, char *value)
{
    if (p == NULL)
    {
        p = malloc(sizeof(btree));
        p->value = value;
        p->left = NULL;
        p->right = NULL;
    }
    else if (strcmp(value, p->value) < 0)
        p->left = add_node(p->left, value);
    else if (strcmp(value, p->value) > 0)
        p->right = add_node(p->right, value);
    return p;
}

void print_tree(btree *p)
{
    if (p == NULL) return;
    print_tree(p->left);
    printf("%s\n", p->value);
    print_tree(p->right);
}
