#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/curator.h"
#include "../headers/utilities.h"
#include "../headers/redblacktree.h"
#include "../headers/voter.h"

#define BUFFER_SIZE 15

struct curator
{
    FILE *inputfile;
    FILE *outputfile;
    RedBlackTree RBT;
};

char *readNextWord(FILE *input) {
    // Ignore whitespace
    char ch;
    while (!feof(input) && ((ch = fgetc(input)) < 'A' || ('Z' < ch && ch < 'a') || ch > 'z'));
    if (feof(input))
        return NULL;
    ungetc(ch,input);
    char *buf;
    int curBufSize = sizeof(char) * BUFFER_SIZE;
    if ((buf = (char*)malloc(curBufSize)) == NULL) {
        not_enough_memory();
        return NULL;
    }
    int i = 0;
    // Read word to dynamically allocated buffer letter by letter in order to avoid static array 
    while (!feof(input) && (ch = fgetc(input)) != '\n' && ch != ' ')
    {
        // Buffer full so allocate more space
        if (i == curBufSize) {
            if ((buf = (char*)realloc(buf,(BUFFER_SIZE + curBufSize) * sizeof(char))) == NULL) {
                not_enough_memory();
                return NULL;
            }
            printf("More buffer memory...\n");
            curBufSize += BUFFER_SIZE * sizeof(char);
        }
        buf[i++] = ch;
    }
    // Buffer full so allocate 1 more byte for end of string character to fit
    if (i == curBufSize) {
        if ((buf = (char*)realloc(buf,(1 + curBufSize) * sizeof(char))) == NULL) {
            not_enough_memory();
            return NULL;
        }
        printf("More buffer memory for end of string character...\n");
        curBufSize += sizeof(char);
    }
    buf[i] = '\0';
    char *word;
    if((word = (char*)malloc((strlen(buf) + 1) * sizeof(char))) == NULL) {
        not_enough_memory();
        return NULL;
    }
    strcpy(word,buf);
    free(buf);
    return word;
}

char readGender(FILE *input) {
    char ch;
    // Ignore whitespace
    while (!feof(input) && ((ch = fgetc(input)) < 'A' || ('Z' < ch && ch < 'a') || ch > 'z'));
    return ch;
}

int Curator_Initialize(Curator *cur,FILE *inputfile,FILE *outputfile) {
    // Allocate memory for the curator
    if ((*cur = (Curator)malloc(sizeof(struct curator))) == NULL) {
        not_enough_memory();
        return 0;
    }
    (*cur)->inputfile = inputfile;
    (*cur)->outputfile = outputfile;
    // Initialize the RBT
    if (!RBT_Initialize(&((*cur)->RBT))) {
        not_enough_memory();
        return 0;
    }
    // Read the records of all voters and insert them to the data structures
    char gender;
    char *idCode,*firstname,*lastname;
    int age,zip;
    while ((idCode = readNextWord(inputfile)) != NULL)
    {
        if ((firstname = readNextWord(inputfile)) == NULL) {
            return 0;
        }
        if ((lastname = readNextWord(inputfile)) == NULL) {
            return 0;
        }
        fscanf(inputfile,"%d",&age);
        gender = readGender(inputfile);
        fscanf(inputfile,"%d",&zip);
        // Allocate memory for the new voter
        Voter v;
        Voter_Initialize(&v,idCode,firstname,lastname,age,gender,zip);
        // Insert her/him to the RBT
        RBT_Insert((*cur)->RBT,v);
        // Free unecessary memory
        free(idCode);
        free(firstname);
        free(lastname);
    }
    return 1;
}

void Curator_Run(Curator cur) {
    int run = 1;
    char *cmd = NULL,*param = NULL;
    // Main Loop(Commands)
    while (run) {
        // Read Command
        printf(">");
        cmd = readNextWord(stdin);
        // 1. lbf key
        // 2. lrb key
        if (!strcmp(cmd,"lrb")) {
            // Read key
            param = readNextWord(stdin);
            if (RBT_Search(cur->RBT,param) != NULL) {
                printf("\t# KEY %s FOUND-IN-RBT\n",param);
            } else {
                printf("\t# KEY %s NOT-IN-RBT\n",param);
            }
        }
        // 11. exit
        else if (!strcmp(cmd,"exit")) {
            run = 0;
        } 
        // Wrong
        else {
            printf("Wrong Command\n");
        }
        if (param != NULL) {
            free(param);
            param = NULL;
        }
        free(cmd);
        cmd = NULL;
    }
}

void Curator_Destroy(Curator *cur) {
    RBT_Destroy(&(*cur)->RBT);
    free(*cur);
    *cur = NULL;
}