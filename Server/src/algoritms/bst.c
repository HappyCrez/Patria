#include "algoritms/bst.h"

void bst_init(struct bst *tree, compare_func cmp_func)
{
        tree->root = NULL;
        tree->cmp_func = cmp_func;
}

/**
 * Recursive realization bst_destroy
 * @node: ptr struct bst
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
        tree->root = NULL;
}

/**
 * Create and return new bst_node
 * @key: value
 * Return: ptr on new struct bst_node or NULL
 */
static struct bst_node *bst_create_node(struct pair *key)
{
        struct bst_node *node = malloc(sizeof(struct bst_node));
        if (!node)
                return NULL; /* Malloc error */

        node->key = key;
        node->left = NULL;
        node->right = NULL;

        return node;
}

/* Recursive realization -> bst_insert */
static struct bst_node *bst_insert_node(struct bst_node *node, compare_func cmp_func, struct pair *key)
{
        if (!node)
                return bst_create_node(key);

        if (cmp_func(key, node->key) < 0)
                node->left = bst_insert_node(node->left, cmp_func, key);
        else if (cmp_func(key, node->key) > 0)
                node->right = bst_insert_node(node->right, cmp_func, key);
        /* if key in already in the tree -> do nothing */

        return node;
}

struct bst_node *bst_insert(struct bst *tree, struct pair *key)
{
        struct bst_node *node = bst_insert_node(tree->root, tree->cmp_func, key);
        if (!tree->root)
                tree->root = node;
        return node;
}

/* Recursive realization -> bst_search */
static struct bst_node *bst_search_node(struct bst_node *node, compare_func cmp_func, struct pair *key)
{
        if (!node)
                return NULL;

        if (cmp_func(key, node->key) == 0)
                return node;

        if (cmp_func(key, node->key) < 0)
                return bst_search_node(node->left, cmp_func, key);

        return bst_search_node(node->right, cmp_func, key);
}

struct bst_node *bst_search(struct bst *tree, struct pair *key)
{
        return bst_search_node(tree->root, tree->cmp_func, key);
}

void postorder_traversal(struct bst_node *node)
{
        if (!node)
                return;

        postorder_traversal(node->left);
        postorder_traversal(node->right);
        printf("%s\n", (char *)node->key->first);
}

struct bst_node *bst_delete_node(struct bst_node *root, struct pair *key)
{
    struct bst_node *temp;

    if (!root)
        return NULL;

    if (key < root->key) {
        root->left = bst_delete_node(root->left, key);
    } else if (key > root->key) {
        root->right = bst_delete_node(root->right, key);
    } else {
        if (!root->left) {
            temp = root->right;
            free(root);
            return temp;
        } else if (!root->right) {
            temp = root->left;
            free(root);
            return temp;
        }

        temp = bst_min_node(root->right);
        root->key = temp->key;
        root->right = bst_delete_node(root->right, temp->key);
    }

    return root;
}