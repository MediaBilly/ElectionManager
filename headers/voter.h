#ifndef VOTER_H
#define VOTER_H

#include "../headers/utilities.h"

typedef struct voter *Voter;

int Voter_Initialize(Voter*,string,string,string,int,char,int);
string Voter_Get_IdCode(Voter);
string Voter_Get_FirstName(Voter);
string Voter_Get_LastName(Voter);
int Voter_Get_Age(Voter);
char Voter_Get_Gender(Voter);
int Voter_Get_Zip(Voter);
void Voter_Print(Voter);
void Voter_Destroy(Voter*);

#endif