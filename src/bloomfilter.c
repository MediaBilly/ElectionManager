#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/bloomfilter.h"
#include "../headers/utilities.h"

#define hash1(str,size) hash(str,256,size)
#define hash2(str,size) hash(str,128,size)
#define hash3(str,size) hash(str,149,size)

#define CEIL(a,b) (((a)+(b)-1)/(b))

struct bloomfilter
{
    unsigned int size;
    unsigned int words;
    char *bitstring;
};

int isPrime(unsigned int num) {
    unsigned int i;
    for (i=2;i*i<= num;i++) {
        if (num % i == 0)
            return 0;
    }
    return 1;
}

unsigned int getSize(int wordsToInsert) {
    unsigned int size = (3*wordsToInsert) % 2 == 0 ? 3*wordsToInsert + 1 : 3*wordsToInsert;
    while (1) {
        if (isPrime(size))
            return size;
        else
            size += 2;
    }
    return size;
}

int BF_Initialize(BloomFilter *bf,unsigned int wordsToInsert) {
    // Allocate memory for the bloom filter
    if ((*bf = (BloomFilter)malloc(sizeof(struct bloomfilter))) == NULL) {
        not_enough_memory();
        return 0;
    }
    // Set the size of the bitstring equal to the first prime number which is at least 3 times the wordsToInsert
    (*bf)->size = getSize(wordsToInsert);
    (*bf)->words = 0;
    // Allocate space for the bit string
    if (((*bf)->bitstring = (char*)malloc(CEIL((*bf)->size * sizeof(char),8*sizeof(char)))) == NULL) {
        not_enough_memory();
        return 0;
    }
    memset((*bf)->bitstring,0,CEIL((*bf)->size * sizeof(char),8*sizeof(char)));
    return 1;
}

unsigned long long hash(char *str,int charbase,unsigned int size) {
    // Compute str key
    int i;
    unsigned long long k = 0,l = strlen(str),base = 1;
    for (i = l - 1;i >= 0;i--) {
        k = (k % size + (base * (str[i] % size)) % size) % size;
        base = (base * (charbase % size)) % size;
    }
    // Return global hash
    return k;
}

int BF_Insert(BloomFilter bf,string str) {
    unsigned long long h1 = hash1(str,bf->size),h2 = hash2(str,bf->size),h3 = hash3(str,bf->size);
    if (str != NULL) {
        bf->bitstring[h1 >> 3] |= ((char)1 << (h1 % (8*sizeof(char))));
        bf->bitstring[h2 >> 3] |= ((char)1 << (h2 % (8*sizeof(char))));
        bf->bitstring[h3 >> 3] |= ((char)1 << (h3 % (8*sizeof(char))));
        bf->words++;
        return 1;
    } else {
        return 0;
    }
}

void BF_Delete(BloomFilter bf) {
    if (bf != NULL) {
        bf->words--;
    }
}

int BF_Search(BloomFilter bf,string str) {
    if (str != NULL) {
        unsigned long long h1 = hash1(str,bf->size),h2 = hash2(str,bf->size),h3 = hash3(str,bf->size);
        unsigned long long a,b,c;
        a = (bf->bitstring[h1 >> 3] & ((char)1 << (h1 % (8*sizeof(char)))));
        b = (bf->bitstring[h2 >> 3] & ((char)1 << (h2 % (8*sizeof(char)))));
        c = (bf->bitstring[h3 >> 3] & ((char)1 << (h3 % (8*sizeof(char)))));
        return a && b && c;
    } else {
        return 0;
    }
}

int BF_Resize(BloomFilter bf) {
    if (bf != NULL) {
        // Calculate new size
        bf->size = getSize(bf->words);
        // Allocate more memory for the bloom filter
        if ((bf->bitstring = (char*)realloc(bf->bitstring,((bf->size) * sizeof(char))/(8*sizeof(char)))) == NULL) {
            not_enough_memory();
            return 0;
        }
        //Clear the bitstring to insert again all the elements
        memset(bf->bitstring,0,(bf->size * sizeof(char))/(8*sizeof(char)));
        return 1;
    } else {
        return 0;
    }
}

int BF_Destroy(BloomFilter *bf) {
    if (*bf != NULL) {
        free((*bf)->bitstring);
        free(*bf);
        *bf = NULL;
        return 1;
    } else {
        return 0;
    }
}
