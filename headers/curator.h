#ifndef CURATOR_H
#define CURATOR_H

#include <stdio.h>

typedef struct curator *Curator;

int Curator_Initialize(Curator*,FILE*,FILE*,int);
void Curator_Run(Curator);
void Curator_Destroy(Curator*);

#endif