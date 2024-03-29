#ifndef POSTCODES_H
#define POSTCODES_H

#include "../headers/voter.h"
#include "../headers/utilities.h"

typedef struct postcodes *PostCodes;

int PostCodes_Initialize(PostCodes*);
int PostCodes_InsertVoter(PostCodes,Voter);
void PostCodes_DeleteVoter(PostCodes,Voter);
void PostCodes_PrintPostCode(PostCodes,int);
void PostCodes_PrintAll(PostCodes);
void PostCodes_PrintFinal(PostCodes,FILE*);
int PostCodes_Destroy(PostCodes*);

#endif