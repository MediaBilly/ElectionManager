#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/utilities.h"
#include "../headers/voter.h"
#include "../headers/redblacktree.h"

// NOTE: The implementation of the RedBlackTree is based on the implementation(in pseudocode) from the book Introduction to Algorithms, 3rd edition 
// by Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest and Clifford Stein (CLRS).

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
    int numVoted;
    RBTNode root;
    RBTNode nullNode;
    RBTNode current;
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
    (*tree)->current = (*tree)->nullNode;
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

void rotateRight(RedBlackTree tree,RBTNode x) {
    RBTNode y = x->left;
    x->left = y->right;
    if (y->right != tree->nullNode) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nullNode) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

void rebalanceAfterInsertion(RedBlackTree tree,RBTNode z) {
    while (z->parent->col == RED) {
        // Parent of z is in the left subtree of the grandparent of z
        if (z->parent == z->parent->parent->left) {
            // y is the uncle of z
            RBTNode y = z->parent->parent->right;
            // Case 1: Uncle of z is red
            if (y->col == RED) {
                z->parent->col = BLACK;
                y->col = BLACK;
                z->parent->parent->col = RED;
                // Move on to the uncle after recoloring him red (in order not to violate property 5)
                z = z->parent->parent;
            } else {
                // Case 2: Uncle of z is black and z is the right child of it's parent
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(tree,z);
                }
                // Case 3: Uncle of z is black and z is the left child of it's parent
                z->parent->col = BLACK;
                z->parent->parent->col = RED;
                rotateRight(tree,z->parent->parent);
            }
        }
        // Same cases but this time parent of z is in the left subtree of the grandparent of z
        else {
            // y is the uncle of z
            RBTNode y = z->parent->parent->left;
            // Case 1: Uncle of z is red
            if (y->col == RED) {
                z->parent->col = BLACK;
                y->col = BLACK;
                z->parent->parent->col = RED;
                // Move on to the uncle after recoloring him red (in order not to violate property 5)
                z = z->parent->parent;
            } else {
                // Case 2: Uncle of z is black and z is the left child of it's parent
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(tree,z);
                }
                // Case 3: Uncle of z is black and z is the right child of it's parent
                z->parent->col = BLACK;
                z->parent->parent->col = RED;
                rotateLeft(tree,z->parent->parent);
            }
        }
    }
    // Final step:color root black in order not to violate property 2
    tree->root->col = BLACK;
}

int RBT_Insert(RedBlackTree tree,Voter v) {
    RBTNode y = tree->nullNode;
    RBTNode x = tree->root;
    // Get the location for the new node to be inserted to based on the bst order property
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
    //First node
    if (y == tree->nullNode) {
        tree->root = z;
    } else if (strcmp(Voter_Get_IdCode(z->value),Voter_Get_IdCode(y->value)) < 0) {
        y->left = z; // Node will be inserted left to the parent
    } else {
        y->right = z; // Node will be inserted right to the parent
    }
    z->left = z->right = tree->nullNode;
    z->col = RED;
    rebalanceAfterInsertion(tree,z);
    return 1;
}

Voter RBT_Search(RedBlackTree tree,string id) {
    RBTNode x = tree->root;
    while (x != tree->nullNode)
    {
        // Id of wanted voter is less than the current voter id so look at the left subtree
        if (strcmp(id,Voter_Get_IdCode(x->value)) < 0) {
            x = x->left;
        } else if (strcmp(id,Voter_Get_IdCode(x->value)) > 0) {
            // Id of wanted voter is less than the current voter id so look at the right subtree
            x = x->right;
        } else {
            break;
        }
    }
    return x->value;
}

RBTNode searchNode(RedBlackTree tree,string id) {
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

// Get the next available idCode stored in the RBT
Voter RBT_Next_Record(RedBlackTree tree) {
    if (tree->current != tree->nullNode) {
        if (tree->current->right != tree->nullNode) {
            tree->current = tree->current->right;
            while (tree->current->left != tree->nullNode)
                tree->current = tree->current->left;
        } else {
            while (tree->current->parent != tree->nullNode && tree->current == tree->current->parent->right)
                tree->current = tree->current->parent;
            tree->current = tree->current->parent;
        }
    } else {
        tree->current = tree->root;
        while (tree->current->left != tree->nullNode)
            tree->current = tree->current->left;
    }
    return  tree->current != tree->nullNode ? tree->current->value : NULL;
}

int RBT_Vote(RedBlackTree tree,string id) {
    //Check if tree is initialized
    if (tree != NULL) {
        // Search for the node with the voter that has the wanted id code
        RBTNode node;
        if ((node = searchNode(tree,id)) != tree->nullNode) {
            if (!Voter_HasVoted(node->value)) {
                Voter_Vote(node->value);
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
    while (subtree->left != tree->nullNode)
        subtree = subtree->left;
    return subtree;
}

void DestroyNode(RBTNode node) {
    Voter_Destroy(&node->value);
    free(node);
}

void rebalanceAfterDeletion(RedBlackTree tree,RBTNode x) {
    while (x != tree->root && x->col == BLACK) {
        // x is the left child of it's parent
        if (x == x->parent->left) {
            // Sibling(brother) of x
            RBTNode w = x->parent->right;
            // Case 1: Sibling w of x is red
            if (w->col == RED) {
                w->col = BLACK;
                x->parent->col = RED;
                rotateLeft(tree,x->parent);
                w = x->parent->right;
            }
            // Case 2: Sibling w of x is black and both w's children are black
            if (w->left->col == BLACK && w->right->col == BLACK) {
                w->col = RED;
                x = x->parent;
            // Case 3: Sibling w of x is black, it's left child is red and it's right child is black
            } else if (w->right->col == BLACK) {
                w->left->col = BLACK;
                w->col = RED;
                rotateRight(tree,w);
                w = x->parent->right;
            }
            // Case 4: Sibling w of x is black, and it's right child is red
            w->col = x->parent->col;
            x->parent->col = BLACK;
            w->right->col = BLACK;
            rotateLeft(tree,x->parent);
            x = tree->root;
        } 
        // Same cases but this time x is the left child of it's parent
        else {
            // Sibling(brother) of x
            RBTNode w = x->parent->left;
            // Case 1: Sibling w of x is red
            if (w->col == RED) {
                w->col = BLACK;
                x->parent->col = RED;
                rotateRight(tree,x->parent);
                w = x->parent->left;
            }
            // Case 2: Sibling w of x is black and both w's children are black
            if (w->right->col == BLACK && w->left->col == BLACK) {
                w->col = RED;
                x = x->parent;
            }
            // Case 3: Sibling w of x is black, it's left child is red and it's right child is black
            else if (w->left->col == BLACK) {
                w->right->col = BLACK;
                w->col = RED;
                rotateLeft(tree,w);
                w = x->parent->left;
            }
            // Case 4: Sibling w of x is black, and it's right child is red
            w->col = x->parent->col;
            x->parent->col = BLACK;
            w->left->col = BLACK;
            rotateRight(tree,x->parent);
            x = tree->root;
        }
        x->col = BLACK;
    }
}

int RBT_Delete(RedBlackTree tree,string id) {
    // Get the node which has the voter with the wanted idCode
    RBTNode z,x,y;
    if ((z = searchNode(tree,id)) != tree->nullNode) {
        y = z;
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
            y = findMinNode(tree,z->right);
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
        if (Voter_HasVoted(z->value)) {
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
