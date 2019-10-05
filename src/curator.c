#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/curator.h"

#define BUFFER_SIZE 20

struct curator
{
    FILE *inputfile;
    FILE *outputfile;
};

char *readNextWord(FILE *input) {
    // Ignore whitespace
    char ch;
    while (!feof(input) && ((ch = fgetc(input)) < 'A' || 'Z' < ch && ch < 'a' || ch > 'z'));
    if (feof(input))
        return NULL;
    ungetc(ch,input);
    char *buf = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    int i = 0;
    // Read word to dynamically allocated buffer letter by letter in order to avoid static array 
    while (!feof(input) && (ch = fgetc(input)) != '\n' && ch != ' ')
    {
        // Buffer full so allocate more space
        if (i == sizeof(buf)/sizeof(char)) {
            int newsize = sizeof(buf) + BUFFER_SIZE;
            buf = (char*)realloc(buf,newsize);
        }
        buf[i++] = ch;
    }
    buf[i] = '\0';
    char *word = (char*)malloc(strlen(buf) * sizeof(char));
    strcpy(word,buf);
    free(buf);
    return word;
}

char readSex(FILE *input) {
    char ch;
    // Ignore whitespace
    while (!feof(input) && ((ch = fgetc(input)) < 'A' || 'Z' < ch && ch < 'a' || ch > 'z'));
    return ch;
}

int Curator_Initialize(Curator *cur,FILE *inputfile,FILE *outputfile) {
    *cur = (Curator)malloc(sizeof(struct curator));
    (*cur)->inputfile = inputfile;
    (*cur)->outputfile = outputfile;
    char sex;
    char *idCode,*firstname,*lastname;
    int age,zip,count = 0;
    while ((idCode = readNextWord(inputfile)) != NULL)
    {
        firstname = readNextWord(inputfile);
        lastname = readNextWord(inputfile);
        fscanf(inputfile,"%d",&age);
        sex = readSex(inputfile);
        fscanf(inputfile,"%d",&zip);
        printf("%s %s %s %d %c %d\n",idCode,firstname,lastname,age,sex,zip);
        free(idCode);
        free(firstname);
        free(lastname);
        count++;
    }
    printf("%d\n",count);
}

void Curator_Run(Curator cur) {

}

void Curator_Destroy(Curator *cur) {
    free(*cur);
    *cur = NULL;
}