#pragma once
#include "dependencies.h"

typedef int(*compare_func) (struct pair *key1, struct pair *key2);

/**
 * Binary search tree Node
 * Contain key as struct pair
 */ 
struct bst_node
{
        char *key;
        int val;
        struct bst_node *left;
        struct bst_node *right;
};


/**
 * Binary search tree
 * Contain compare function 
 */ 
struct bst
{
        struct bst_node *root;
        compare_func cmp_func; /* Compare function*/
};

/**
 * Init struct bst
 * @tree: ptr on struct bst
 */
void bst_init(struct bst *tree);

/**
 * Insert pair {key,val} in bst
 * @tree: ptr on struct bst
 * @data: pair {key,value}
 * Return: ptr on root or NULL
 */
struct bst_node *bst_insert(struct bst *tree, struct pair *data);

/**
 * Search key in bst
 * @tree: ptr on struct bst
 * @key: value
 * Return: ptr on struct bst_node or NULL
 */
struct bst_node *bst_search(struct bst *tree, char *key);

/**
 * Destroy tree
 * @tree: ptr on struct bst
 */
void bst_destroy(struct bst *tree);

/**
 * Print tree
 * @node: ptr on struct node bst
 */
void bst_postorder_traversal(struct bst_node *node);

/**
 * Delete node
 * @bst: ptr on struct bst where to find key for delete operation
 * @key: ptr on struct pair, what we want to delete from bst
 */
void bst_delete_node(struct bst *ptr, char *key);