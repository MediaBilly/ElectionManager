#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H

#include "../headers/voter.h"
#include "../headers/utilities.h"

// RBT_VOTE results
#define VOTE_SUCCESS 1
#define ALREADY_VOTED 0
#define VOTER_NOT_FOUND -1

typedef struct redblacktree *RedBlackTree;

int RBT_Initialize(RedBlackTree*);
int RBT_Insert(RedBlackTree,Voter);
Voter RBT_Search(RedBlackTree,string);
Voter RBT_Next_Record(RedBlackTree);
int RBT_Vote(RedBlackTree,string);
int RBT_NumVoted(RedBlackTree);
int RBT_Delete(RedBlackTree,string);
int RBT_Destroy(RedBlackTree*);

#endif