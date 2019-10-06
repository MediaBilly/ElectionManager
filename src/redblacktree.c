#include <stdio.h>
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
    RBTNode nullNode;
};

int RBT_Initialize(RedBlackTree *tree) {
    // Allocate memory for the RBT
    if ((*tree = (RedBlackTree)malloc(sizeof(struct redblacktree))) == NULL) {
        not_enough_memory();
        return 0;
    }
    // Allocate memory for the nullNode
    if (((*tree)->nullNode = (RBTNode)malloc(sizeof(struct rbtnode))) == NULL) {
        not_enough_memory();
        return 0;
    }
    (*tree)->nullNode->col = BLACK;
    (*tree)->nullNode->value = NULL;
    (*tree)->nullNode->parent = (*tree)->nullNode;
    (*tree)->nullNode->left = (*tree)->nullNode;
    (*tree)->nullNode->right = (*tree)->nullNode;
    (*tree)->size = 0;
    (*tree)->numVoted = 0;
    (*tree)->root = (*tree)->nullNode;
    return 1;
}

void rotateLeft(RedBlackTree tree,RBTNode x) {
    RBTNode y = x->right;
    x->right = y->left;
    if (y->left != tree->nullNode) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nullNode) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rotateRight(RedBlackTree tree,RBTNode y) {
    RBTNode x = y->left;
    y->left = x->right;
    if (x->right != tree->nullNode) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == tree->nullNode) {
        tree->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

void rebalanceAfterInsertion(RedBlackTree tree,RBTNode z) {
    while (z->parent->col == RED) {
        if (z->parent == z->parent->parent->left) {
            RBTNode y = z->parent->parent->right;
            if (y->col == RED) {
                z->parent->col = BLACK;
                y->col = BLACK;
                z->parent->parent->col = RED;
                z = z->parent->parent;
            } else if (z == z->parent->right) {
                z = z->parent;
                rotateLeft(tree,z);
            } else {
                z->parent->col = BLACK;
                z->parent->parent->col = RED;
                rotateRight(tree,z->parent->parent);
            }
        } else {
            RBTNode y = z->parent->parent->left;
            if (y->col == RED) {
                z->parent->col = BLACK;
                y->col = BLACK;
                z->parent->parent->col = RED;
                z = z->parent->parent;
            } else if (z == z->parent->left) {
                z = z->parent;
                rotateRight(tree,z);
            } else {
                z->parent->col = BLACK;
                z->parent->parent->col = RED;
                rotateLeft(tree,z->parent->parent);
            }
        }
    }
    tree->root->col = BLACK;
}

int RBT_Insert(RedBlackTree tree,Voter v) {
    RBTNode y = tree->nullNode;
    RBTNode x = tree->root;
    // Get the location for the nrw node to be inserted to based on the bst order property
    while (x != tree->nullNode) {
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
    z->left = z->right = tree->nullNode;
    z->voted = 0;
    //First node
    if (y == tree->nullNode) {
        tree->root = z;
    } else if (strcmp(Voter_Get_IdCode(z->value),Voter_Get_IdCode(y->value)) < 0) {
        y->left = z; // Node will be inserted left to the parent
    } else {
        y->right = z; // Node will be inserted right to the parent
    }
    z->col = RED;
    rebalanceAfterInsertion(tree,z);
    return 1;
}

Voter RBT_Search(RedBlackTree tree,char *id) {
    RBTNode x = tree->root;
    while (x != tree->nullNode)
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

RBTNode searchNode(RedBlackTree tree,char *id) {
    RBTNode root = tree->root;
    while (root != tree->nullNode) {
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
        if ((node = searchNode(tree,id)) != tree->nullNode) {
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
    if (u->parent == tree->nullNode) {
        // u is root so v becomes the new one
        tree->root = v;
    } else if (u == u->parent->left) {
        // u is the left subtree of it's parent
        u->parent->left = v;
    } else {
        // u is the right subtree of it's parent
        u->parent->right = v;
    }
    // v's parent is now u's parent
    v->parent = u->parent;
}

RBTNode findMinNode(RedBlackTree tree, RBTNode subtree) {
    while (subtree != tree->nullNode)
        subtree = subtree->left;
    return subtree;
}

void DestroyNode(RBTNode node) {
    Voter_Destroy(&node->value);
    free(node);
}

void rebalanceAfterDeletion(RedBlackTree tree,RBTNode x) {
    while (x != tree->root && x->col == BLACK) {
        if (x == x->parent->left) {
            RBTNode w = x->parent->right;
            if (w->col == RED) {
                w->col = BLACK;
                x->parent->col = RED;
                rotateLeft(tree,x->parent);
                w = x->parent->right;
            }
            if (w->left->col == BLACK && w->right->col == BLACK) {
                w->col = RED;
                x = x->parent;
            } else if (w->right->col == BLACK) {
                w->left->col = BLACK;
                w->col = RED;
                rotateRight(tree,w);
                w = x->parent->right;
            } else {
                w->col = x->parent->col;
                x->parent->col = BLACK;
                w->right->col = BLACK;
                rotateLeft(tree,x->parent);
                x = tree->root;
            }
        } else {
            RBTNode w = x->parent->left;
            if (w->col == RED) {
                w->col = BLACK;
                x->parent->col = RED;
                rotateRight(tree,x->parent);
                w = x->parent->left;
            }
            if (w->right->col == BLACK && w->left->col == BLACK) {
                w->col = RED;
                x = x->parent;
            } else if (w->left->col == BLACK) {
                w->right->col = BLACK;
                w->col = RED;
                rotateLeft(tree,w);
                w = x->parent->left;
            } else {
                w->col = x->parent->col;
                x->parent->col = BLACK;
                w->left->col = BLACK;
                rotateRight(tree,x->parent);
                x = tree->root;
            }
        }
        x->col = BLACK;
    }
}

int RBT_Delete(RedBlackTree tree,char *id) {
    // Get the node which has the voter with the wanted idCode
    RBTNode z,x;
    if ((z = searchNode(tree,id)) != tree->nullNode) {
        RBTNode y = z;
        Color ycolor = y->col;
        if (z->left == tree->nullNode) {
            // The node to be deleted has either a right child or no children
            // Put the right child in the place of the toDel node
            x = z->right;
            Transform(tree,z,z->right);
        } else if (z->right == tree->nullNode) {
            // The node to be deleted has either a left child or no children
            x = z->left;
            Transform(tree,z,z->left);
        } else {
            // The node to be deleted has 2 children
            RBTNode y = findMinNode(tree,z->right);
            ycolor = y->col;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                Transform(tree,y,y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            Transform(tree,z,y);
            y->left = z->left;
            y->left->parent = y;
            y->col = z->col;
        }
        if (ycolor == BLACK) {
            rebalanceAfterDeletion(tree,x);
        }
        // Destroy toDel node
        if (z->voted) {
            tree->numVoted--;
        }
        DestroyNode(z);
    } else {
        // Voter with the wanted idCode does not exist
        return 0;
    }
    return 1;
}

void Destroy(RedBlackTree tree,RBTNode node) {
    if (node != tree->nullNode) {
        Destroy(tree,node->left);
        Destroy(tree,node->right);
        DestroyNode(node);
    }
}

int RBT_Destroy(RedBlackTree *tree) {
    // Not initialized
    if (*tree == NULL) {
        return 0;
    }
    Destroy(*tree,(*tree)->root);
    DestroyNode((*tree)->nullNode);
    free(*tree);
    *tree = NULL;
    return 1;
}
