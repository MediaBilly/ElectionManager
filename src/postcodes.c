#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/postcodes.h"
#include "../headers/voter.h"
#include "../headers/utilities.h"

typedef struct listnode *ListNode;

struct listnode
{
    Voter v;
    ListNode next;
};

typedef struct postcodechainnode *PostCodeChainNode;

struct postcodechainnode
{
    int postcode;
    ListNode voterList;
    PostCodeChainNode next;
};

struct postcodes
{
    int buckets;
    PostCodeChainNode *table;
};

int PostCodes_Initialize(PostCodes *pc) {
    // Allocate memory for the postcodes hashtable
    if ((*pc = (PostCodes)malloc(sizeof(struct postcodes))) == NULL) {
        not_enough_memory();
        return 0;
    }
    // Size of hashtable is first prime after 3*10000
    (*pc)->buckets = 30011;
    // Allocate memory for the PostCodeChainNode
    if (((*pc)->table = (PostCodeChainNode*)malloc((*pc)->buckets * sizeof(PostCodeChainNode))) == NULL) {
        not_enough_memory();
        return 0;
    }
    int i;
    for (i = 0;i < (*pc)->buckets;i++)
        (*pc)->table[i] = NULL;
    return 1;
}

unsigned long long hashPostCode(int postcode,PostCodes pc) {
    return ((unsigned long long)8353 * postcode + (unsigned long long)4793) % pc->buckets;
}

int PostCodes_InsertVoter(PostCodes pc,Voter v) {
    // Calculate hash for voter postcode
    if (v == NULL)
        return 0;
    int postcode = Voter_Get_Zip(v);
    unsigned long long h = hashPostCode(postcode,pc);
    if (pc->table[h] != NULL) {
        // Search if postcode is already inserted
        PostCodeChainNode tmp = pc->table[h];
        while (tmp != NULL && tmp->postcode != postcode)
            tmp = tmp->next;
        // Found
        if (tmp != NULL) {
            // Allocate memory for the new voter node
            ListNode tmpNode;
            if ((tmpNode = (ListNode)malloc(sizeof(struct listnode))) == NULL) {
                not_enough_memory();
                return 0;
            }
            // Assign pointers
            tmpNode->v = v;
            tmpNode->next = tmp->voterList;
            tmp->voterList = tmpNode;
        } else {
            // Not found so insert new postcode node and voter node at start of it's nodelist
            // Allocate memory for the new postcode node
            tmp = pc->table[h];
            if ((pc->table[h] = (PostCodeChainNode)malloc(sizeof(struct postcodechainnode))) == NULL) {
                not_enough_memory();
                return 0;
            }
            // Assign pointers
            pc->table[h]->next = tmp;
            pc->table[h]->postcode = postcode;
            // Allocate memory for the new voter node
            if ((pc->table[h]->voterList = (ListNode)malloc(sizeof(struct listnode))) == NULL) {
                not_enough_memory();
                return 0;
            }
            // Assign pointers
            pc->table[h]->voterList->v = v;
            pc->table[h]->voterList->next = NULL;
        }
    } else {
        // Not found so insert new postcode node and voter node at start of it's nodelist
        // Allocate memory for the new postcode node
        if ((pc->table[h] = (PostCodeChainNode)malloc(sizeof(struct postcodechainnode))) == NULL) {
            not_enough_memory();
            return 0;
        }
        // Assign pointers
        pc->table[h]->next = NULL;
        pc->table[h]->postcode = postcode;
        // Allocate memory for the new voter node
        if ((pc->table[h]->voterList = (ListNode)malloc(sizeof(struct listnode))) == NULL) {
            not_enough_memory();
            return 0;
        }
        // Assign pointers
        pc->table[h]->voterList->v = v;
        pc->table[h]->voterList->next = NULL;
    }
    return 1;
}

void PostCodes_DeleteVoter(PostCodes pc,Voter v) {
    // Calculate hash for postcode
    unsigned long long h = hashPostCode(Voter_Get_Zip(v),pc);
    // Search for the list of the voters who have the wanted postcode
    PostCodeChainNode pcNode = pc->table[h];
    while (pcNode != NULL && pcNode->postcode != Voter_Get_Zip(v)) 
        pcNode = pcNode->next;
    if (pcNode != NULL) {
        ListNode node = pcNode->voterList;
        // Search for the wanted voter
        // If he is the first one in the list just delete him
        if (!strcmp(Voter_Get_IdCode(node->v),Voter_Get_IdCode(v))) {
            // Make the list start pointing to the next node
            pcNode->voterList = pcNode->voterList->next;
            // Delete the old first node
            free(node);
        } else {
            // Otherwise look for him in the rest of the list
            ListNode prev = node;
            node = node->next;
            while (node != NULL && strcmp(Voter_Get_IdCode(node->v),Voter_Get_IdCode(v))) {
                prev = prev->next;
                node = node->next;
            }
            // If found delete him
            if (node != NULL) {
                prev->next = node->next;
                free(node);
            }
        }
    }
}

int CountVotersWithPostCode(PostCodeChainNode pcNode) {
    // If found iterate through all it's voters and count them
    int voters = 0;
    if (pcNode != NULL) {
        ListNode voterNode = pcNode->voterList;
        while (voterNode != NULL) {
            if (Voter_HasVoted(voterNode->v)) {
                voters++;
            }
            voterNode = voterNode->next;
        }
        // Print amount of people with the current postcode who have voted
    }
    return voters;
}

void PostCodes_PrintPostCode(PostCodes pc,int postcode) {
    // Calculate hash for postcode
    unsigned long long h = hashPostCode(postcode,pc);
    // Search for the list of the voters who have the wanted postcode
    PostCodeChainNode pcNode = pc->table[h];
    while (pcNode != NULL && pcNode->postcode != postcode) 
        pcNode = pcNode->next;
    int voters = pcNode != NULL ? CountVotersWithPostCode(pcNode) : 0;
    printf("\t# IN %d VOTERS-ARE %d\n",postcode,voters);
}

void PostCodes_PrintAll(PostCodes pc) {
    // Iterate through all buckets of postcode chains
    int i = 0;
    PostCodeChainNode pcNode;
    for (i = 0; i < pc->buckets; i++) {
        // Print voters for each postcode in the current bucket
        pcNode = pc->table[i];
        while (pcNode != NULL) {
            printf("\t# IN %d VOTERS-ARE %d\n",pcNode->postcode,CountVotersWithPostCode(pcNode));
            pcNode = pcNode->next;
        }
    }
}

int PostCodes_Destroy(PostCodes *pc) {
    if (*pc != NULL) {
        // For each chain destroy all chain nodes
        int i;
        PostCodeChainNode pcNode,toDelPcNode;
        ListNode node,toDelNode;
        for (i = 0; i < (*pc)->buckets; i++) {
            // For each chain(postcode) node of the current bucket destroy all voter nodes 
            pcNode = (*pc)->table[i];
            while (pcNode != NULL) {
                node = pcNode->voterList;
                while (node != NULL) {
                    toDelNode = node;
                    node = node->next;
                    free(toDelNode);
                }
                toDelPcNode = pcNode;
                pcNode = pcNode->next;
                free(toDelPcNode);
            }
        }
        // Destroy hash table
        free((*pc)->table);
        // Destroy post codes data structure itself
        free(*pc);
        *pc = NULL;
        return 1;
    } else {
        return 0;
    }
}
