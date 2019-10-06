#ifndef VOTER_H
#define VOTER_H

typedef struct voter *Voter;

int Voter_Initialize(Voter*,char*,char*,char*,int,char,int);
char *Voter_Get_IdCode(Voter);
char *Voter_Get_FirstName(Voter);
char *Voter_Get_LastName(Voter);
int Voter_Get_Age(Voter);
char Voter_Get_Gender(Voter);
int Voter_Get_Zip(Voter);
void Voter_Print(Voter);
void Voter_Destroy(Voter*);

#endif