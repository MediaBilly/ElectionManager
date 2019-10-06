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
    int voted;
};

struct redblacktree
{
    int size;
    int numVoted;
    RBTNode root;
};

int RBT_Initialize(RedBlackTree *tree) {
    if ((*tree = (RedBlackTree)malloc(sizeof(struct redblacktree))) == NULL) {
        not_enough_memory();
        return 0;
    }
    (*tree)->size = 0;
    (*tree)->numVoted = 0;
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
    z->voted = 0;
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

RBTNode searchNode(RBTNode root,char *id) {
    while (root != NULL) {
        // Id of wanted voter is less than the current voter id so look at the left subtree
        if (strcmp(id,Voter_Get_IdCode(root->value)) < 0) {
            root = root->left;
        } else if (strcmp(id,Voter_Get_IdCode(root->value)) > 0) {
            // Id of wanted voter is less than the current voter id so look at the left subtree
            root = root->right;
        } else {
            break;
        }
    }
    return root;
}

int RBT_Vote(RedBlackTree tree,char *id) {
    //Check if tree is initialized
    if (tree != NULL) {
        // Search for the node with the voter that has the wanted id code
        RBTNode node;
        if ((node = searchNode(tree->root,id)) != NULL) {
            if (!node->voted) {
                node->voted = 1;
                tree->numVoted++;
                return VOTE_SUCCESS;
            } else {
                return ALREADY_VOTED;
            }
        } else {
            return VOTER_NOT_FOUND;
        }
    } else {
        return -2;
    }
}

int RBT_NumVoted(RedBlackTree tree) {
    return tree->numVoted;
}

// Put v subtree int the place of u subtree
void Transform(RedBlackTree tree,RBTNode u,RBTNode v) {
    if (u->parent == NULL) {
        // u is root so v becomes the new one
        tree->root = v;
    } else if (u == u->parent->left) {
        // u is the left subtree of it's parent
        u->parent->left = v;
    } else {
        // u is the right subtree of it's parent
        u->parent->right = v;
    }
    if (v != NULL) {
        // v's parent is now u's parent
        v->parent = u->parent;
    }
}

RBTNode findMinNode(RBTNode subtree) {
    while (subtree != NULL)
        subtree = subtree->left;
    return subtree;
}

void DestroyNode(RBTNode node) {
    Voter_Destroy(&node->value);
    free(node);
}

int RBT_Delete(RedBlackTree tree,char *id) {
    // Get the node which has the voter with the wanted idCode
    RBTNode toDel;
    if ((toDel = searchNode(tree->root,id)) != NULL) {
        if (toDel->left == NULL) {
            // The node to be deleted has either a right child or no children
            // Put the right child in the place of the toDel node
            Transform(tree,toDel,toDel->right);
        } else if (toDel->right == NULL) {
            // The node to be deleted has either a left child or no children
            Transform(tree,toDel,toDel->left);
        } else {
            // The node to be deleted has 2 children
            RBTNode y = findMinNode(toDel->right);
            if (y->parent != toDel) {
                Transform(tree,y,y->right);
                y->right = toDel->right;
                y->right->parent = y;
            }
            Transform(tree,toDel,y);
            y->left = toDel->left;
            y->left->parent = y;
        }
        // Destroy toDel node
        if (toDel->voted) {
            tree->numVoted--;
        }
        DestroyNode(toDel);
    } else {
        // Voter with the wanted idCode does not exist
        return 0;
    }
    return 1;
}

void Destroy(RBTNode node) {
    if (node != NULL) {
        Destroy(node->left);
        Destroy(node->right);
        DestroyNode(node);
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
