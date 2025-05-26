#pragma once
#include "dependencies.h"

typedef int(*compare_func) (struct pair *key1, struct pair *key2);

/**
 * Binary search tree Node
 * Contain key as struct pair
 */ 
struct bst_node
{
        struct pair *key;
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
void bst_init(struct bst *tree, compare_func cmp_func);

/**
 * Insert key in bst
 * @tree: ptr on struct bst
 * @key: value
 * Return: ptr on new bst_node or NULL
 */
struct bst_node *bst_insert(struct bst *tree, struct pair *key);

/**
 * Search key in bst
 * @tree: ptr on struct bst
 * @key: value
 * Return: ptr on struct bst_node or NULL
 */
struct bst_node *bst_search(struct bst *tree, struct pair *key);

/**
 * Destroy tree
 * @tree: ptr on struct bst
 */
void bst_destroy(struct bst *tree);