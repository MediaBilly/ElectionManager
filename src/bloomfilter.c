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

// Calculates (base^exp) mod modulus which is required for miller rabin primility test
// Algorithm Used:Modular Exponentiation Right To Left Binary Method
/*
    Sources:
    https://stackoverflow.com/questions/19839457/explanation-of-right-to-left-binary-method-of-modular-arithmetic
    https://en.wikipedia.org/wiki/Modular_exponentiation
*/
unsigned long long modpow(unsigned long long base,unsigned long long exp,unsigned long long modulus)
{
    unsigned long long result = 1;
    while (exp > 0)
    {
        if(exp % 2 == 1)
            result = (result * base) % modulus;
        exp >>= 1;
        base = (base * base) % modulus;
    }
    return result;
}

// Use miller-rabin primility test to check whether num is prime
// Source:https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
int isPrime(unsigned int num) {
    unsigned int d = num - 1,r = 0,a,k,i,ok,loop = 1;
    unsigned long long x;
    while (d % 2 == 0)
    {
        r++;
        d/=2;
    }
    for(a = 2;loop;a = a == 2 ? 7 : 61)
    {
        k = (d - 1)/2;
        x = (a * modpow((a * a) % num,k,num)) % num;
        if(x == 1 || x == num - 1)
        {
            if(a == 61)
                break;
            else
                continue;
        }
        ok = 1;
        for(i = 0; i < r - 1 && ok;i++)
        {
            x = (x * x) % num;
            if(x == num - 1)
                ok = 0;
        }
        if(ok)
            return 0;
        if(a == 61)
            loop = 0;
    }
    return 1;
    /*int i;
    for (i = 2;i*i <= num;i++)
        if (num % i == 0)
            return 0;
    return 1;*/
}

unsigned int getSize(int wordsToInsert) {
    unsigned int size = 2;
    while (1)
    {
        if(size == 2)
        {
            if(size >= 3 * wordsToInsert)
                return size;
            size++;
        }
        else if(size == 3)
        {
            if(size >= 3 * wordsToInsert)
                return size;
            size += 2;
        }
        else
        {
            if(size >= 3 * wordsToInsert && isPrime(size))
                return size;
            if(size + 2 >= 3 * wordsToInsert && isPrime(size + 2))
                return size + 2;
            size += 6;
        }
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
