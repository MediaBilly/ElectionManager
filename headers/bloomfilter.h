#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include "../headers/utilities.h"

typedef struct bloomfilter *BloomFilter;

int BF_Initialize(BloomFilter*,unsigned int);
int BF_Insert(BloomFilter,string);
int BF_Search(BloomFilter,string);
int BF_Resize(BloomFilter);
int BF_Destroy(BloomFilter*);

#endif