#ifndef POSTCODES_H
#define POSTCODES_H

#include "../headers/voter.h"

typedef struct postcodes *PostCodes;

int PostCodes_Initialize(PostCodes*);
int PostCodes_InsertVoter(PostCodes,Voter);
void PostCodes_PrintPostCode(PostCodes,int);
void PostCodes_PrintAll(PostCodes);
int PostCodes_Destroy(PostCodes*);

#endif