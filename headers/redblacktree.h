#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H

#include "../headers/voter.h"

typedef struct redblacktree *RedBlackTree;

int RBT_Initialize(RedBlackTree*);
int RBT_Insert(RedBlackTree,Voter);
Voter RBT_Search(RedBlackTree,char*);
int RBT_Delete(RedBlackTree,char*);
int RBT_Destroy(RedBlackTree*);

#endif