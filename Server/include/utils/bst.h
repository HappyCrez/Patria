/**
 * @file  bst.h
 * @brief universal binary search tree to contain any data by its pointer and
 * compare function
 */

#pragma once
#include "dependencies.h"

typedef int (*compare_func)(void *data1, void *data2);

/**
 * @brief struct of node in bst
 * @param data is void pointer to any data what you need
 * @param left pointer on left node
 * @param right pointer on right node
 */
struct bst_node
{
        void *data;             /* private field */
        struct bst_node *left;  /* private field */
        struct bst_node *right; /* private field */
};

/**
 * @brief binary search tree
 * @param root pointer on root node
 * @param cmp_func pointer on compare function
 */
struct bst
{
        struct bst_node *root;
        compare_func cmp_func; /* private field */
};

/**
 * @brief initialize struct bst
 * @param func function to compare void *data
 * @return pointer on new bst
 */
struct bst *bst_init(compare_func func);

/**
 * @brief insert data in bst
 * @param tree pointer on bst
 * @param data pointer on data to insert
 * @return pointer on root (if insert operation failed, return null)
 */
struct bst_node *bst_insert(struct bst *tree, void *data);

/**
 * @brief search data in bst
 * @param tree pointer on struct bst
 * @param data value to find
 * @return pointer on struct bst_node (if not founded, return null)
 */
struct bst_node *bst_search(struct bst *tree, void *data);

/**
 * @brief destroy bst
 * @tree: pointer on bst to destroy
 */
void bst_destroy(struct bst *tree);

/**
 * @brief delete node by data
 * @param bst pointer on bst where delete data
 * @param data pointer on data, what you want to delete from bst
 */
void bst_delete_node(struct bst *ptr, void *data);

typedef int (*print_func)(void *data);

/**
 * @brief print tree in stdout (Debug function)
 * @param node output start (to see all bst, pass root node)
 * @param print function to print data from nodes
 * @todo debug function may be removed in next versions
 */
void bst_postorder_traversal(struct bst_node *node, print_func print);