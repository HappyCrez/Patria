#include "algoritms/bst.h"

void bst_init(struct bst *tree)
{
        tree->root = NULL;
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
static struct bst_node *bst_create_node(struct pair *data)
{
        struct bst_node *node = malloc(sizeof(struct bst_node));
        if (!node)
                return NULL; /* Malloc error */

        node->key = (char *)data->first;
        node->val = data->second;
        node->left = NULL;
        node->right = NULL;

        return node;
}

/* Recursive realization -> bst_insert */
static struct bst_node *bst_insert_node(struct bst_node *node, struct pair *data)
{
        if (!node)
                return bst_create_node(data);

        if (strcmp((char *)data->first, node->key) < 0)
                node->left = bst_insert_node(node->left, data);
        else if (strcmp((char *)data->first, node->key) > 0)
                node->right = bst_insert_node(node->right, data);
                
        /* if key in already in the tree -> do nothing */
        return node;
}

struct bst_node *bst_insert(struct bst *tree, struct pair *data)
{
        struct bst_node *node = bst_insert_node(tree->root, data);

        if (!tree->root)
                tree->root = node;
        return node;
}

/* Recursive realization -> bst_search */
static struct bst_node *bst_search_node(struct bst_node *node, char *key)
{
        if (!node)
                return NULL;

        if (strcmp(key, node->key) == 0)
                return node;

        if (strcmp(key, node->key) < 0)
                return bst_search_node(node->left, key);

        return bst_search_node(node->right, key);
}

struct bst_node *bst_search(struct bst *tree, char *key)
{
        return bst_search_node(tree->root, key);
}

void bst_postorder_traversal(struct bst_node *node)
{
        if (!node)
                return;

        bst_postorder_traversal(node->left);
        bst_postorder_traversal(node->right);
        printf("%s : %d\n", (char *)node->key, node->val);
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
static struct bst_node *bst_delete_node_recursive(struct bst_node *root, char *key)
{
    struct bst_node *temp;

    if (!root)
        return NULL;

    if (strcmp(key, root->key) < 0) {
        root->left = bst_delete_node_recursive(root->left, key);
    } else if (strcmp(key, root->key) > 0) {
        root->right = bst_delete_node_recursive(root->right, key);
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
        root->right = bst_delete_node_recursive(root->right, temp->key);
    }
    return root;
}

void bst_delete_node(struct bst *ptr, char *key)
{
        ptr->root = bst_delete_node_recursive(ptr->root, key);
}