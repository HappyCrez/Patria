#include "utils/bst.h"

struct bst *bst_init(compare_func func)
{
        struct bst *bst = malloc(sizeof(bst));
        bst->root = null;
        bst->cmp_func = func;
}

/**
 * @brief recursive realization bst_destroy
 * @param node pointer on next deletion node
 */
static void bst_destroy_node(struct bst_node *node)
{
        if (!node)
                return;

        bst_destroy_node(node->left);
        bst_destroy_node(node->right);
        free(node);
}

void bst_destroy(struct bst *tree)
{
        bst_destroy_node(tree->root);
        tree->root = null;
}

/**
 * @brief create and return new bst_node
 * @param key value to write in node
 * @return pointer on new bst_node (or null)
 */
static struct bst_node *bst_create_node(struct pair *data)
{
        struct bst_node *node = malloc(sizeof(struct bst_node));
        if (!node)
                return null; /* Malloc error */

        node->data = data;
        node->left = null;
        node->right = null;

        return node;
}

/* Recursive realization -> bst_insert */
static struct bst_node *bst_insert_node(struct bst_node *node, compare_func cmp, void *data)
{
        if (!node)
                return bst_create_node(data);

        int compare_result = cmp(node->data, data);
        if (compare_result < 0)
                node->left = bst_insert_node(node->left, cmp, data);
        else if (compare_result > 0)
                node->right = bst_insert_node(node->right, cmp, data);

        /* if key in already in the tree -> do nothing */
        return node;
}

struct bst_node *bst_insert(struct bst *tree, void *data)
{
        struct bst_node *node = bst_insert_node(tree->root, tree->cmp_func, data);

        if (!tree->root)
                tree->root = node;
        return node;
}

/* Recursive realization -> bst_search */
static struct bst_node *bst_search_node(struct bst_node *node, compare_func cmp, void *data)
{
        if (!node)
                return null;

        int compare_result = cmp(node->data, data);
        if (compare_result == 0)
                return node;

        if (compare_result < 0)
                return bst_search_node(node->left, cmp, data);

        return bst_search_node(node->right, cmp, data);
}

struct bst_node *bst_search(struct bst *tree, void *data)
{
        return bst_search_node(tree->root, tree->cmp_func, data);
}

/* Find last left node in tree */
static struct bst_node *bst_min_node(struct bst_node *node)
{
        while (node && node->left)
                node = node->left;
        return node;
}

/**
 * Delete node
 * @root: ptr on root node
 * @key: ptr on key
 * Return: ptr on new root
 */
static struct bst_node *bst_delete_node_recursive(struct bst_node *root, compare_func cmp, void *data)
{
        struct bst_node *temp;

        if (!root)
                return null;

        int compare_result = cmp(root->data, data);
        if (compare_result < 0)
        {
                root->left = bst_delete_node_recursive(root->left, cmp, data);
        }
        else if (compare_result > 0)
        {
                root->right = bst_delete_node_recursive(root->right, cmp, data);
        }
        else
        {
                if (!root->left)
                {
                        temp = root->right;
                        free(root);
                        return temp;
                }
                else if (!root->right)
                {
                        temp = root->left;
                        free(root);
                        return temp;
                }

                temp = bst_min_node(root->right);
                root->data = temp->data;
                root->right = bst_delete_node_recursive(root->right, cmp, temp->data);
        }
        return root;
}

void bst_delete_node(struct bst *bst, void *data)
{
        bst->root = bst_delete_node_recursive(bst->root, bst->cmp_func, data);
}

void bst_postorder_traversal(struct bst_node *node, print_func print)
{
        if (!node)
                return;

        bst_postorder_traversal(node->left, print);
        bst_postorder_traversal(node->right, print);
        print(node->data);
}