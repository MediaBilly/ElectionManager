#include <stdlib.h>
#include <string.h>
#include "../headers/utilities.h"
#include "../headers/voter.h"
#include "../headers/redblacktree.h"

typedef struct rbtnode *RBTNode;

typedef enum color{BLACK, RED} Color;

struct rbtnode
{
    Voter value;
    Color col;
    RBTNode parent;
    RBTNode left;
    RBTNode right;
};

struct redblacktree
{
    int size;
    RBTNode root;
};

int RBT_Initialize(RedBlackTree *tree) {
    if ((*tree = (RedBlackTree)malloc(sizeof(struct redblacktree))) == NULL) {
        not_enough_memory();
        return 0;
    }
    (*tree)->size = 0;
    (*tree)->root = NULL;
    return 1;
}

int RBT_Insert(RedBlackTree tree,Voter v) {
    RBTNode y = NULL;
    RBTNode x = tree->root;
    // Get the location for the nrw node to be inserted to based on the bst order property
    while (x != NULL){
        y = x;
        // IdCode of new voter is less than the IdCode of the current node so insert to the left subtree
        if (strcmp(Voter_Get_IdCode(v),Voter_Get_IdCode(x->value)) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    //Allocate memory for the new node
    RBTNode z;
    if ((z = (RBTNode)malloc(sizeof(struct rbtnode))) == NULL) {
        not_enough_memory();
        return 0;
    }
    // Assign it's values
    z->parent = y;
    z->value = v;
    z->left = z->right = NULL;
    //First node
    if (y == NULL) {
        tree->root = z;
    } else if (strcmp(Voter_Get_IdCode(z->value),Voter_Get_IdCode(y->value)) < 0) {
        y->left = z; // Node will be inserted left to the parent
    } else {
        y->right = z; // Node will be inserted right to the parent
    }
    return 1;
}

Voter RBT_Search(RedBlackTree tree,char *id) {
    RBTNode x = tree->root;
    while (x != NULL)
    {
        // Id of wanted voter is less than the current voter id so look at the left subtree
        if (strcmp(id,Voter_Get_IdCode(x->value)) < 0) {
            x = x->left;
        } else if (strcmp(id,Voter_Get_IdCode(x->value)) > 0) {
            // Id of wanted voter is less than the current voter id so look at the left subtree
            x = x->right;
        } else {
            break;
        }
    }
    return x != NULL ? x->value : NULL;
}

int RBT_Delete(RedBlackTree tree,char *id) {
    // TODO
    return 0;
}

void Destroy(RBTNode node) {
    if (node != NULL) {
        Destroy(node->left);
        Destroy(node->right);
        Voter_Destroy(&node->value);
        free(node);
    }
}

int RBT_Destroy(RedBlackTree *tree) {
    // Not initialized
    if (*tree == NULL) {
        return 0;
    }
    Destroy((*tree)->root);
    free(*tree);
    *tree = NULL;
    return 1;
}
